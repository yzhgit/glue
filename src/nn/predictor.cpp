//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/nn/predictor.h"

#ifdef GLUE_USE_TENSORRT
    #include "glue/nn/predictor_tensorrt.h"
#endif

#ifdef GLUE_USE_NCNN
    #include "glue/nn/predictor_ncnn.h"
#endif

#ifdef GLUE_USE_SNPE
    #include "glue/nn/predictor_snpe.h"
#endif

#define TAG "Predictor"
#define PRINT(...) INFERENCE_HELPER_LOG_PRINT(TAG, __VA_ARGS__)
#define PRINT_E(...) INFERENCE_HELPER_LOG_PRINT_E(TAG, __VA_ARGS__)

namespace glue {


Predictor *Predictor::Create(const Predictor::PredictorType type) {
    Predictor *p = nullptr;
    switch (type) {
#ifdef GLUE_USE_TENSORRT
    case kTensorrt:
        PRINT("Use TensorRT \n");
        p = new PredictorTensorRt();
        break;
#endif
#ifdef GLUE_USE_NCNN
    case kNcnn:
    case kNcnnVulkan:
        PRINT("Use NCNN\n");
        p = new PredictorNcnn();
        break;
#endif
#ifdef GLUE_USE_MNN
    case kMnn:
        PRINT("Use MNN\n");
        p = new InferenceHelperMnn();
        break;
#endif
#ifdef GLUE_USE_SNPE
    case kSnpe:
        PRINT("Use SNPE\n");
        p = new PredictorSnpe();
        break;
#endif
    default:
        PRINT_E("Unsupported inference helper type (%d)\n", helper_type);
        break;
    }
    if (p == nullptr) {
        PRINT_E("Failed to create inference helper\n");
    } else {
        p->helper_type_ = helper_type;
    }
    return p;
}

void Predictor::ConvertNormalizeParameters(InputTensorInfo &tensor_info) {
    if (tensor_info.data_type != InputTensorInfo::kDataTypeImage)
        return;

#if 0
    /* Convert to speeden up normalization:  ((src / 255) - mean) / norm  = src * 1 / (255 * norm) - (mean / norm) */
    for (int32_t i = 0; i < 3; i++) {
        tensor_info.normalize.mean[i] /= tensor_info.normalize.norm[i];
        tensor_info.normalize.norm[i] *= 255.0f;
        tensor_info.normalize.norm[i] = 1.0f / tensor_info.normalize.norm[i];
    }
#endif
#if 1
    /* Convert to speeden up normalization:  ((src / 255) - mean) / norm = (src
     * - (mean * 255))  * (1 / (255 * norm)) */
    for (int32_t i = 0; i < 3; i++) {
        tensor_info.normalize.mean[i] *= 255.0f;
        tensor_info.normalize.norm[i] *= 255.0f;
        tensor_info.normalize.norm[i] = 1.0f / tensor_info.normalize.norm[i];
    }
#endif
}

void Predictor::PreProcessImage(int32_t num_thread,
                                const InputTensorInfo &input_tensor_info,
                                float *dst) {
    const int32_t img_width = input_tensor_info.GetWidth();
    const int32_t img_height = input_tensor_info.GetHeight();
    const int32_t img_channel = input_tensor_info.GetChannel();
    uint8_t *src = (uint8_t *)(input_tensor_info.data);
    if (input_tensor_info.is_nchw == true) {
        /* convert NHWC to NCHW */
#pragma omp parallel for num_threads(num_thread)
        for (int32_t c = 0; c < img_channel; c++) {
            for (int32_t i = 0; i < img_width * img_height; i++) {
                dst[c * img_width * img_height + i] =
                    (src[i * img_channel + c] -
                     input_tensor_info.normalize.mean[c]) *
                    input_tensor_info.normalize.norm[c];
            }
        }
    } else {
        /* convert NHWC to NHWC */
#pragma omp parallel for num_threads(num_thread)
        for (int32_t i = 0; i < img_width * img_height; i++) {
            for (int32_t c = 0; c < img_channel; c++) {
#if 1
                dst[i * img_channel + c] =
                    (src[i * img_channel + c] -
                     input_tensor_info.normalize.mean[c]) *
                    input_tensor_info.normalize.norm[c];
#else
                dst[i * img_channel + c] =
                    (src[i * img_channel + c] / 255.0f -
                     input_tensor_info.normalize.mean[c]) /
                    input_tensor_info.normalize.norm[c];
#endif
            }
        }
    }
}

void Predictor::PreProcessImage(int32_t num_thread,
                                const InputTensorInfo &input_tensor_info,
                                uint8_t *dst) {
    const int32_t img_width = input_tensor_info.GetWidth();
    const int32_t img_height = input_tensor_info.GetHeight();
    const int32_t img_channel = input_tensor_info.GetChannel();
    uint8_t *src = (uint8_t *)(input_tensor_info.data);
    if (input_tensor_info.is_nchw == true) {
        /* convert NHWC to NCHW */
#pragma omp parallel for num_threads(num_thread)
        for (int32_t c = 0; c < img_channel; c++) {
            for (int32_t i = 0; i < img_width * img_height; i++) {
                dst[c * img_width * img_height + i] = src[i * img_channel + c];
            }
        }
    } else {
        /* convert NHWC to NHWC */
        std::copy(src, src + input_tensor_info.GetElementNum(), dst);
    }
}

void Predictor::PreProcessImage(int32_t num_thread,
                                const InputTensorInfo &input_tensor_info,
                                int8_t *dst) {
    const int32_t img_width = input_tensor_info.GetWidth();
    const int32_t img_height = input_tensor_info.GetHeight();
    const int32_t img_channel = input_tensor_info.GetChannel();
    uint8_t *src = (uint8_t *)(input_tensor_info.data);
    if (input_tensor_info.is_nchw == true) {
        /* convert NHWC to NCHW */
#pragma omp parallel for num_threads(num_thread)
        for (int32_t c = 0; c < img_channel; c++) {
            for (int32_t i = 0; i < img_width * img_height; i++) {
                dst[c * img_width * img_height + i] =
                    src[i * img_channel + c] - 128;
            }
        }
    } else {
#pragma omp parallel for num_threads(num_thread)
        for (int32_t i = 0; i < img_width * img_height; i++) {
            for (int32_t c = 0; c < img_channel; c++) {
                dst[i * img_channel + c] = src[i * img_channel + c] - 128;
            }
        }
    }
}

template <typename T>
void Predictor::PreProcessBlob(int32_t num_thread,
                               const InputTensorInfo &input_tensor_info,
                               T *dst) {
    const int32_t img_width = input_tensor_info.GetWidth();
    const int32_t img_height = input_tensor_info.GetHeight();
    const int32_t img_channel = input_tensor_info.GetChannel();
    T *src = static_cast<T *>(input_tensor_info.data);
    if ((input_tensor_info.data_type == InputTensorInfo::kDataTypeBlobNchw &&
         input_tensor_info.is_nchw) ||
        (input_tensor_info.data_type == InputTensorInfo::kDataTypeBlobNhwc &&
         !input_tensor_info.is_nchw)) {
        std::copy(src, src + input_tensor_info.GetElementNum(), dst);
    } else if (input_tensor_info.data_type ==
               InputTensorInfo::kDataTypeBlobNchw) {
        /* NCHW -> NHWC */
#pragma omp parallel for num_threads(num_thread)
        for (int32_t i = 0; i < img_width * img_height; i++) {
            for (int32_t c = 0; c < img_channel; c++) {
                dst[i * img_channel + c] =
                    src[c * (img_width * img_height) + i];
            }
        }
    } else if (input_tensor_info.data_type ==
               InputTensorInfo::kDataTypeBlobNhwc) {
        /* NHWC -> NCHW */
#pragma omp parallel for num_threads(num_thread)
        for (int32_t i = 0; i < img_width * img_height; i++) {
            for (int32_t c = 0; c < img_channel; c++) {
                dst[c * (img_width * img_height) + i] =
                    src[i * img_channel + c];
            }
        }
    }
}

template void Predictor::PreProcessBlob<float>(
    int32_t num_thread, const InputTensorInfo &input_tensor_info, float *dst);
template void Predictor::PreProcessBlob<int32_t>(
    int32_t num_thread, const InputTensorInfo &input_tensor_info, int32_t *dst);
template void Predictor::PreProcessBlob<int64_t>(
    int32_t num_thread, const InputTensorInfo &input_tensor_info, int64_t *dst);
template void Predictor::PreProcessBlob<uint8_t>(
    int32_t num_thread, const InputTensorInfo &input_tensor_info, uint8_t *dst);
template void Predictor::PreProcessBlob<int8_t>(
    int32_t num_thread, const InputTensorInfo &input_tensor_info, int8_t *dst);


}
