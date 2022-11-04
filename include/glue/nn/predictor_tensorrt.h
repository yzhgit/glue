//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/* for My modules */
#include "glue/nn/predictor.h"

namespace nvinfer1 {
class IRuntime;
class ICudaEngine;
class IExecutionContext;
} // namespace nvinfer1

GLUE_START_NAMESPACE
namespace ml {

class PredictorTensorRt : public Predictor {
  public:
    PredictorTensorRt();
    ~PredictorTensorRt() override;
    int32_t SetNumThreads(const int32_t num_threads) override;
    int32_t
    Initialize(const std::string &model_filename,
               std::vector<InputTensorInfo> &input_tensor_info_list,
               std::vector<OutputTensorInfo> &output_tensor_info_list) override;
    int32_t Finalize(void) override;
    int32_t PreProcess(
        const std::vector<InputTensorInfo> &input_tensor_info_list) override;
    int32_t
    Process(std::vector<OutputTensorInfo> &output_tensor_info_list) override;
    void SetDlaCore(int32_t dla_core) { dla_core_ = dla_core; }

  private:
    int32_t
    AllocateBuffers(std::vector<InputTensorInfo> &input_tensor_info_list,
                    std::vector<OutputTensorInfo> &output_tensor_info_list);

  private:
    int32_t num_threads_;
    int32_t dla_core_;
    std::unique_ptr<nvinfer1::IRuntime> runtime_;
    std::unique_ptr<nvinfer1::ICudaEngine> engine_;
    std::unique_ptr<nvinfer1::IExecutionContext> context_;
    std::vector<std::pair<void *, int32_t>>
        buffer_list_cpu_; // pointer and size (can be overwritten by user)
    std::vector<std::pair<void *, int32_t>>
        buffer_list_cpu_reserved_; // pointer and size (fixed in initialization)
    std::vector<void *> buffer_list_gpu_;
};

} // namespace ml
GLUE_END_NAMESPACE
