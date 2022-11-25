//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "tensorrt/predictor_impl.h"

#include <NvInfer.h>
#include <NvOnnxParser.h>
#include <cuda_runtime_api.h>

#include "tensorrt/common.h"

#define USE_FP16

#define OPT_MAX_WORK_SPACE_SIZE ((size_t)1 << 30)

namespace glue {

PredictorImpl::~PredictorImpl() = default;

PredictorImpl::PredictorImpl() : num_threads_(1), dla_core_(-1) {}

int PredictorImpl::SetNumThreads(int num_threads) {
    num_threads_ = num_threads;
    return kRetOk;
}

int PredictorImpl::Initialize(const std::string& model_filename,
                              std::vector<TensorInfo>& input_tensor_info_list,
                              std::vector<TensorInfo>& output_tensor_info_list) {
    bool is_trt_model = false;
    bool is_onnx_model = false;

    std::string trt_model_filename = std::string(model_filename);
    if (model_filename.find(".onnx") != std::string::npos) {
        is_onnx_model = true;
        trt_model_filename = trt_model_filename.replace(
            trt_model_filename.find(".onnx"), std::string(".onnx").length(), ".trt\0");
        std::ifstream ifs(trt_model_filename);
        if (ifs.is_open()) {
            is_onnx_model = false;
            is_trt_model = true;
        }
    } else if (trt_model_filename.find(".trt") != std::string::npos) {
        is_trt_model = true;
    } else {
        printf("unsupoprted file format (%s)\n", model_filename.c_str());
        return kRetErr;
    }

    /*** create runtime ***/
    (void)initLibNvInferPlugins(nullptr, "");
    runtime_ = std::unique_ptr<nvinfer1::IRuntime>(
        nvinfer1::createInferRuntime(sample::gLogger.getTRTLogger()));
    if (!runtime_) {
        printf("Failed to create runtime (%s)\n", model_filename.c_str());
        return kRetErr;
    }

    /*** create engine from model file ***/
    if (is_trt_model) {
        /* Just load TensorRT model (serialized model) */
        std::string buffer;
        std::ifstream stream(trt_model_filename, std::ios::binary);
        if (stream) {
            stream >> std::noskipws;
            copy(std::istream_iterator<char>(stream),
                 std::istream_iterator<char>(),
                 back_inserter(buffer));
        }

        engine_ = std::unique_ptr<nvinfer1::ICudaEngine>(
            runtime_->deserializeCudaEngine(buffer.data(), buffer.size()));
        stream.close();
        if (!engine_) {
            printf("Failed to create engine (%s)\n", trt_model_filename.c_str());
            return kRetErr;
        }
    } else if (is_onnx_model) {
        /* Create a TensorRT model from another format */
        auto builder = std::unique_ptr<nvinfer1::IBuilder>(
            nvinfer1::createInferBuilder(sample::gLogger.getTRTLogger()));
        const auto explicitBatch =
            1U << static_cast<uint32_t>(nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH);
        auto network =
            std::unique_ptr<nvinfer1::INetworkDefinition>(builder->createNetworkV2(explicitBatch));
        auto config = std::unique_ptr<nvinfer1::IBuilderConfig>(builder->createBuilderConfig());

        auto parser = std::unique_ptr<nvonnxparser::IParser>(
            nvonnxparser::createParser(*network, sample::gLogger.getTRTLogger()));
        if (!parser->parseFromFile(model_filename.c_str(),
                                   (int)nvinfer1::ILogger::Severity::kWARNING)) {
            printf("Failed to parse onnx file (%s)", model_filename.c_str());
            return kRetErr;
        }

        config->setMaxWorkspaceSize(OPT_MAX_WORK_SPACE_SIZE);

#if defined(USE_FP16)
        config->setFlag(nvinfer1::BuilderFlag::kFP16);
#else
        config->setFlag(nvinfer1::BuilderFlag::kINT8);
        samplesCommon::setAllDynamicRanges(network.get(), 2.5f, 2.5f);
#endif

        if (dla_core_ >= 0) {
            printf("Use DLA: %d\n", dla_core_);
            samplesCommon::enableDLA(builder.get(), config.get(), dla_core_);
        }

        auto plan = std::unique_ptr<nvinfer1::IHostMemory>(
            builder->buildSerializedNetwork(*network, *config));
        if (!plan) {
            printf("Failed to create plan (%s)\n", model_filename.c_str());
            return kRetErr;
        }

        engine_ = std::unique_ptr<nvinfer1::ICudaEngine>(
            runtime_->deserializeCudaEngine(plan->data(), plan->size()));
        if (!engine_) {
            printf("Failed to create engine (%s)\n", model_filename.c_str());
            return kRetErr;
        }

        /* save serialized model for next time */
        std::ofstream ofs(std::string(trt_model_filename), std::ios::out | std::ios::binary);
        ofs.write((char*)(plan->data()), plan->size());
        ofs.close();
    }

    context_ = std::unique_ptr<nvinfer1::IExecutionContext>(engine_->createExecutionContext());
    if (!context_) {
        printf("Failed to create context (%s)\n", model_filename.c_str());
        return kRetErr;
    }

    /* Allocate host/device buffers and assign to tensor info */
    for (auto& input_tensor_info : input_tensor_info_list) {
        input_tensor_info.id = -1;  // not assigned
    }
    for (auto& output_tensor_info : output_tensor_info_list) {
        output_tensor_info.id = -1;  // not assigned
    }
    if (AllocateBuffers(input_tensor_info_list, output_tensor_info_list) != kRetOk) {
        return kRetErr;
    }
    /* Check if the tensor is assigned (exists in the model) */
    for (auto& input_tensor_info : input_tensor_info_list) {
        if (input_tensor_info.id == -1) {
            printf("Input tensor doesn't exist in the model (%s)\n",
                   input_tensor_info.name.c_str());
            return kRetErr;
        }
    }
    for (auto& output_tensor_info : output_tensor_info_list) {
        if (output_tensor_info.id == -1) {
            printf("Output tensor doesn't exist in the model (%s)\n",
                   output_tensor_info.name.c_str());
            return kRetErr;
        }
    }

    /* Convert normalize parameter to speed up */
    for (auto& input_tensor_info : input_tensor_info_list) {
        ConvertNormalizeParameters(input_tensor_info);
    }

    return kRetOk;
}

int PredictorImpl::Finalize(void) {
    int num_of_in_out = engine_->getNbBindings();
    for (int i = 0; i < num_of_in_out; i++) {
        const auto data_type = engine_->getBindingDataType(i);
        switch (data_type) {
            case nvinfer1::DataType::kFLOAT:
            case nvinfer1::DataType::kHALF:
            case nvinfer1::DataType::kINT32:
                delete[](float*)(buffer_list_cpu_[i].first);
                break;
            case nvinfer1::DataType::kINT8:
                delete[](int*)(buffer_list_cpu_[i].first);
                break;
            default:
                return kRetErr;
        }
    }

    for (auto p : buffer_list_gpu_) {
        cudaFree(p);
    }

    return kRetOk;
}

int PredictorImpl::PreProcess(const std::vector<TensorInfo>& input_tensor_info_list) {
    for (const auto& input_tensor_info : input_tensor_info_list) {
        const int32_t img_width = input_tensor_info.GetWidth();
        const int32_t img_height = input_tensor_info.GetHeight();
        const int32_t img_channel = input_tensor_info.GetChannel();
        if (input_tensor_info.data_type == TensorInfo::kDataTypeImage) {
            if ((input_tensor_info.image_info.width != input_tensor_info.image_info.crop_width) ||
                (input_tensor_info.image_info.height != input_tensor_info.image_info.crop_height)) {
                printf("Crop is not supported\n");
                return kRetErr;
            }
            if ((input_tensor_info.image_info.crop_width != img_width) ||
                (input_tensor_info.image_info.crop_height != img_height)) {
                printf("Resize is not supported\n");
                return kRetErr;
            }
            if (input_tensor_info.image_info.channel != img_channel) {
                printf("Color conversion is not supported\n");
                return kRetErr;
            }

            /* Normalize image */
            if (input_tensor_info.tensor_type == TensorInfo::kTensorTypeFp32) {
                float* dst = (float*)(buffer_list_cpu_[input_tensor_info.id].first);
                PreProcessImage(num_threads_, input_tensor_info, dst);
            } else if (input_tensor_info.tensor_type == TensorInfo::kTensorTypeUint8) {
                uint8_t* dst = (uint8_t*)(buffer_list_cpu_[input_tensor_info.id].first);
                PreProcessImage(num_threads_, input_tensor_info, dst);
            } else if (input_tensor_info.tensor_type == TensorInfo::kTensorTypeInt8) {
                int8_t* dst = (int8_t*)(buffer_list_cpu_[input_tensor_info.id].first);
                PreProcessImage(num_threads_, input_tensor_info, dst);
            } else {
                printf("Unsupported tensor_type (%d)\n", input_tensor_info.tensor_type);
                return kRetErr;
            }
        } else if ((input_tensor_info.data_type == TensorInfo::kDataTypeBlobNhwc) ||
                   (input_tensor_info.data_type == TensorInfo::kDataTypeBlobNchw)) {
            if (input_tensor_info.tensor_type == TensorInfo::kTensorTypeFp32) {
                float* dst = (float*)(buffer_list_cpu_[input_tensor_info.id].first);
                PreProcessBlob<float>(num_threads_, input_tensor_info, dst);
            } else if (input_tensor_info.tensor_type == TensorInfo::kTensorTypeUint8 ||
                       input_tensor_info.tensor_type == TensorInfo::kTensorTypeInt8) {
                uint8_t* dst = (uint8_t*)(buffer_list_cpu_[input_tensor_info.id].first);
                PreProcessBlob<uint8_t>(num_threads_, input_tensor_info, dst);
            } else if (input_tensor_info.tensor_type == TensorInfo::kTensorTypeInt32) {
                int32_t* dst = (int32_t*)(buffer_list_cpu_[input_tensor_info.id].first);
                PreProcessBlob<int32_t>(num_threads_, input_tensor_info, dst);
            } else {
                printf("Unsupported tensor_type (%d)\n", input_tensor_info.tensor_type);
                return kRetErr;
            }
        } else {
            printf("Unsupported data_type (%d)\n", input_tensor_info.data_type);
            return kRetErr;
        }
    }
    return kRetOk;
}

int PredictorImpl::Process(std::vector<TensorInfo>& output_tensor_info_list) {
    cudaStream_t stream;
    cudaStreamCreate(&stream);

    for (int i = 0; i < (int)buffer_list_cpu_.size(); i++) {
        if (engine_->bindingIsInput(i)) {
            cudaMemcpyAsync(buffer_list_gpu_[i],
                            buffer_list_cpu_[i].first,
                            buffer_list_cpu_[i].second,
                            cudaMemcpyHostToDevice,
                            stream);
        }
    }
    context_->enqueue(1, &buffer_list_gpu_[0], stream, NULL);
    for (int i = 0; i < (int)buffer_list_cpu_.size(); i++) {
        if (!engine_->bindingIsInput(i)) {
            cudaMemcpyAsync(buffer_list_cpu_[i].first,
                            buffer_list_gpu_[i],
                            buffer_list_cpu_[i].second,
                            cudaMemcpyDeviceToHost,
                            stream);
        }
    }
    cudaStreamSynchronize(stream);

    cudaStreamDestroy(stream);

    (void)output_tensor_info_list;  // no need to set output data, because the ptr
                                    // to output data is already set at initialize

    return kRetOk;
}

int PredictorImpl::AllocateBuffers(std::vector<TensorInfo>& input_tensor_info_list,
                                   std::vector<TensorInfo>& output_tensor_info_list) {
    int32_t num_of_in_out = engine_->getNbBindings();
    printf("num_of_in_out = %d\n", num_of_in_out);

    for (int32_t i = 0; i < num_of_in_out; i++) {
        printf("tensor[%d]->name: %s\n", i, engine_->getBindingName(i));
        printf("  is input = %d\n", engine_->bindingIsInput(i));
        int32_t data_size = 1;
        const auto dims = engine_->getBindingDimensions(i);
        for (int32_t i = 0; i < dims.nbDims; i++) {
            printf("  dims.d[%d] = %d\n", i, dims.d[i]);
            data_size *= dims.d[i];
        }
        const auto data_type = engine_->getBindingDataType(i);
        printf("  data_type = %d\n", static_cast<int32_t>(data_type));

        void* buffer_cpu = nullptr;
        void* buffer_gpu = nullptr;
        switch (data_type) {
            case nvinfer1::DataType::kFLOAT:
            case nvinfer1::DataType::kHALF:
            case nvinfer1::DataType::kINT32:
                buffer_cpu = new float[data_size];
                buffer_list_cpu_.push_back(
                    std::pair<void*, int32_t>(buffer_cpu, data_size * sizeof(float)));
                cudaMalloc(&buffer_gpu, data_size * sizeof(float));
                buffer_list_gpu_.push_back(buffer_gpu);
                break;
            case nvinfer1::DataType::kINT8:
                buffer_cpu = new int8_t[data_size];
                buffer_list_cpu_.push_back(
                    std::pair<void*, int32_t>(buffer_cpu, data_size * sizeof(int8_t)));
                cudaMalloc(&buffer_gpu, data_size * sizeof(int8_t));
                buffer_list_gpu_.push_back(buffer_gpu);
                break;
            default:
                printf("Unsupported datatype (%d)\n", static_cast<int32_t>(data_type));
                return kRetErr;
        }

        if (engine_->bindingIsInput(i)) {
            for (auto& input_tensor_info : input_tensor_info_list) {
                int32_t id = engine_->getBindingIndex(input_tensor_info.name.c_str());
                if (id == i) {
                    input_tensor_info.id = id;
                    if (input_tensor_info.tensor_dims.empty()) {
                        /* tensor size is not assigned. so get from the model */
                        for (int32_t i = 0; i < dims.nbDims; i++) {
                            input_tensor_info.tensor_dims.push_back(dims.d[i]);
                        }
                    } else {
                        /* tensor size is assigned. so check if it's the same as
                         * size defined in the model */
                        if (static_cast<int32_t>(input_tensor_info.tensor_dims.size()) !=
                            dims.nbDims) {
                            printf("Input Tensor dims doesn't match\n");
                            return kRetErr;
                        }
                        for (int32_t i = 0; i < dims.nbDims; i++) {
                            if (input_tensor_info.tensor_dims[i] != dims.d[i]) {
                                printf("Input Tensor size doesn't match\n");
                                return kRetErr;
                            }
                        }
                    }
                    if (((input_tensor_info.tensor_type == TensorInfo::kTensorTypeUint8) &&
                         (data_type == nvinfer1::DataType::kINT8)) ||
                        ((input_tensor_info.tensor_type == TensorInfo::kTensorTypeFp32) &&
                         (data_type == nvinfer1::DataType::kFLOAT)) ||
                        ((input_tensor_info.tensor_type == TensorInfo::kTensorTypeInt32) &&
                         (data_type == nvinfer1::DataType::kINT32))) {
                        /* OK */
                    } else {
                        printf("Input Tensor type doesn't match\n");
                        return kRetErr;
                    }
                }
            }
        } else {
            for (auto& output_tensor_info : output_tensor_info_list) {
                int32_t id = engine_->getBindingIndex(output_tensor_info.name.c_str());
                if (id == i) {
                    output_tensor_info.id = id;
                    if (output_tensor_info.tensor_dims.empty()) {
                        /* tensor size is not assigned. so get from the model */
                        for (int32_t i = 0; i < dims.nbDims; i++) {
                            output_tensor_info.tensor_dims.push_back(dims.d[i]);
                        }
                    } else {
                        /* tensor size is assigned. so check if it's the same as
                         * size defined in the model */
                        if (static_cast<int32_t>(output_tensor_info.tensor_dims.size()) !=
                            dims.nbDims) {
                            printf("Output Tensor dims doesn't match\n");
                            return kRetErr;
                        }
                        for (int32_t i = 0; i < dims.nbDims; i++) {
                            if (output_tensor_info.tensor_dims[i] != dims.d[i]) {
                                printf("Output Tensor size doesn't match\n");
                                return kRetErr;
                            }
                        }
                    }

                    if (((output_tensor_info.tensor_type == TensorInfo::kTensorTypeUint8) &&
                         (data_type == nvinfer1::DataType::kINT8)) ||
                        ((output_tensor_info.tensor_type == TensorInfo::kTensorTypeFp32) &&
                         (data_type == nvinfer1::DataType::kFLOAT)) ||
                        ((output_tensor_info.tensor_type == TensorInfo::kTensorTypeInt32) &&
                         (data_type == nvinfer1::DataType::kINT32))) {
                        /* OK */
                    } else {
                        printf("Output Tensor type doesn't match\n");
                        return kRetErr;
                    }
                    if (data_type == nvinfer1::DataType::kINT8) {
                        output_tensor_info.quant.scale = 1.0;  // todo
                        output_tensor_info.quant.zero_point = 0.0;
                    }
                    output_tensor_info.data = buffer_cpu;
                }
            }
        }
    }

    return kRetOk;
}

}  // namespace glue
