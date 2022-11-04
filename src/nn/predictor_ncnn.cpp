//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

/* for ncnn */
#include "net.h"

/* for My modules */
#include "glue/nn/predictor_ncnn.h"

/*** Macro ***/
#define TAG "PredictorNcnn"
#define PRINT(...) INFERENCE_HELPER_LOG_PRINT(TAG, __VA_ARGS__)
#define PRINT_E(...) INFERENCE_HELPER_LOG_PRINT_E(TAG, __VA_ARGS__)


GLUE_START_NAMESPACE
namespace ml {

PredictorNcnn::PredictorNcnn() {
    num_threads_ = 1;
}

PredictorNcnn::~PredictorNcnn() {}

int32_t PredictorNcnn::SetNumThreads(const int32_t num_threads) {
    num_threads_ = num_threads;
    return kRetOk;
}

int32_t PredictorNcnn::Initialize(
    const std::string &model_filename,
    std::vector<InputTensorInfo> &input_tensor_info_list,
    std::vector<OutputTensorInfo> &output_tensor_info_list) {
    /*** Create network ***/
    net_.reset(new ncnn::Net());
    net_->opt.use_fp16_arithmetic = true;
    net_->opt.use_fp16_packed = true;
    net_->opt.use_fp16_storage = true;
    if (helper_type_ == kNcnnVulkan) {
        net_->opt.use_vulkan_compute = 1;
    }

    std::string bin_filename = model_filename;
    if (model_filename.find(".param") == std::string::npos) {
        PRINT_E("Invalid model param filename (%s)\n", model_filename.c_str());
        return kRetErr;
    }
    bin_filename = bin_filename.replace(
        bin_filename.find(".param"), std::string(".param").length(), ".bin\0");
    if (net_->load_param(model_filename.c_str()) != 0) {
        PRINT_E("Failed to load model param file (%s)\n",
                model_filename.c_str());
        return kRetErr;
    }
    if (net_->load_model(bin_filename.c_str()) != 0) {
        PRINT_E("Failed to load model bin file (%s)\n", bin_filename.c_str());
        return kRetErr;
    }

    /* Convert normalize parameter to speed up */
    for (auto &input_tensor_info : input_tensor_info_list) {
        ConvertNormalizeParameters(input_tensor_info);
    }

    /* Check if tensor info is set */
    for (const auto &input_tensor_info : input_tensor_info_list) {
        for (const auto &dim : input_tensor_info.tensor_dims) {
            if (dim <= 0) {
                PRINT_E("Invalid tensor size\n");
                return kRetErr;
            }
        }
    }

    return kRetOk;
};

int32_t PredictorNcnn::Finalize(void) {
    net_.reset();
    in_mat_list_.clear();
    out_mat_list_.clear();
    if (helper_type_ == kNcnnVulkan) {
        ncnn::destroy_gpu_instance();
    }
    return kRetErr;
}

int32_t PredictorNcnn::PreProcess(
    const std::vector<InputTensorInfo> &input_tensor_info_list) {
    in_mat_list_.clear();
    for (const auto &input_tensor_info : input_tensor_info_list) {
        ncnn::Mat ncnn_mat;
        if (input_tensor_info.data_type == InputTensorInfo::kDataTypeImage) {
            /* Crop */
            if ((input_tensor_info.image_info.width !=
                 input_tensor_info.image_info.crop_width) ||
                (input_tensor_info.image_info.height !=
                 input_tensor_info.image_info.crop_height)) {
                PRINT_E("Crop is not supported\n");
                return kRetErr;
            }
            /* Convert color type */
            int32_t pixel_type = 0;
            if ((input_tensor_info.image_info.channel == 3) &&
                (input_tensor_info.GetChannel() == 3)) {
                pixel_type = (input_tensor_info.image_info.is_bgr)
                                 ? ncnn::Mat::PIXEL_BGR
                                 : ncnn::Mat::PIXEL_RGB;
                if (input_tensor_info.image_info.swap_color) {
                    pixel_type = (input_tensor_info.image_info.is_bgr)
                                     ? ncnn::Mat::PIXEL_BGR2RGB
                                     : ncnn::Mat::PIXEL_RGB2BGR;
                }
            } else if ((input_tensor_info.image_info.channel == 1) &&
                       (input_tensor_info.GetChannel() == 1)) {
                pixel_type = ncnn::Mat::PIXEL_GRAY;
            } else if ((input_tensor_info.image_info.channel == 3) &&
                       (input_tensor_info.GetChannel() == 1)) {
                pixel_type = (input_tensor_info.image_info.is_bgr)
                                 ? ncnn::Mat::PIXEL_BGR2GRAY
                                 : ncnn::Mat::PIXEL_RGB2GRAY;
            } else if ((input_tensor_info.image_info.channel == 1) &&
                       (input_tensor_info.GetChannel() == 3)) {
                pixel_type = ncnn::Mat::PIXEL_GRAY2RGB;
            } else {
                PRINT_E("Unsupported color conversion (%d, %d)\n",
                        input_tensor_info.image_info.channel,
                        input_tensor_info.GetChannel());
                return kRetErr;
            }

            if (input_tensor_info.image_info.crop_width ==
                    input_tensor_info.GetWidth() &&
                input_tensor_info.image_info.crop_height ==
                    input_tensor_info.GetHeight()) {
                /* Convert to blob */
                ncnn_mat = ncnn::Mat::from_pixels(
                    (uint8_t *)input_tensor_info.data, pixel_type,
                    input_tensor_info.image_info.width,
                    input_tensor_info.image_info.height);
            } else {
                /* Convert to blob with resize */
                ncnn_mat = ncnn::Mat::from_pixels_resize(
                    (uint8_t *)input_tensor_info.data, pixel_type,
                    input_tensor_info.image_info.width,
                    input_tensor_info.image_info.height,
                    input_tensor_info.GetWidth(),
                    input_tensor_info.GetHeight());
            }
            /* Normalize image */
            ncnn_mat.substract_mean_normalize(input_tensor_info.normalize.mean,
                                              input_tensor_info.normalize.norm);
        } else if (input_tensor_info.data_type ==
                   InputTensorInfo::kDataTypeBlobNhwc) {
            PRINT_E("[ToDo] Unsupported data type (%d)\n",
                    input_tensor_info.data_type);
            ncnn_mat = ncnn::Mat::from_pixels(
                (uint8_t *)input_tensor_info.data,
                input_tensor_info.GetChannel() == 3 ? ncnn::Mat::PIXEL_RGB
                                                    : ncnn::Mat::PIXEL_GRAY,
                input_tensor_info.GetWidth(), input_tensor_info.GetHeight());
        } else if (input_tensor_info.data_type ==
                   InputTensorInfo::kDataTypeBlobNchw) {
            ncnn_mat = ncnn::Mat(
                input_tensor_info.GetWidth(), input_tensor_info.GetHeight(),
                input_tensor_info.GetChannel(), input_tensor_info.data);
        } else {
            PRINT_E("Unsupported data type (%d)\n",
                    input_tensor_info.data_type);
            return kRetErr;
        }
        in_mat_list_.push_back(std::pair<std::string, ncnn::Mat>(
            input_tensor_info.name, ncnn_mat));
    }
    return kRetOk;
}

int32_t PredictorNcnn::Process(
    std::vector<OutputTensorInfo> &output_tensor_info_list) {
    ncnn::Extractor ex = net_->create_extractor();
    ex.set_light_mode(true);
    ex.set_num_threads(num_threads_);
    for (const auto &inputMat : in_mat_list_) {
        if (ex.input(inputMat.first.c_str(), inputMat.second) != 0) {
            PRINT_E("Input mat error (%s)\n", inputMat.first.c_str());
            return kRetErr;
        }
    }

    out_mat_list_.clear();
    for (auto &output_tensor_info : output_tensor_info_list) {
        ncnn::Mat ncnn_out;
        if (ex.extract(output_tensor_info.name.c_str(), ncnn_out) != 0) {
            PRINT_E("Output mat error (%s)\n", output_tensor_info.name.c_str());
            return kRetErr;
        }
        out_mat_list_.push_back(ncnn_out); // store ncnn mat in member variable
                                           // so that data keep exist
        output_tensor_info.data = ncnn_out.data;
        output_tensor_info.tensor_dims.clear();
        output_tensor_info.tensor_dims.push_back(1);
        output_tensor_info.tensor_dims.push_back(ncnn_out.c);
        output_tensor_info.tensor_dims.push_back(ncnn_out.h);
        output_tensor_info.tensor_dims.push_back(ncnn_out.w);
    }

    return kRetOk;
}

} // namespace ml
GLUE_END_NAMESPACE
