//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/nn/predictor.h"

namespace nvinfer1 {
class IRuntime;
class ICudaEngine;
class IExecutionContext;
}  // namespace nvinfer1

namespace glue {

class PredictorImpl : public Predictor {
   public:
    PredictorImpl();
    ~PredictorImpl();
    int SetNumThreads(int num_threads);
    int Initialize(const std::string& model_filename,
                   std::vector<TensorInfo>& input_tensor_info_list,
                   std::vector<TensorInfo>& output_tensor_info_list);
    int Finalize(void);
    int PreProcess(const std::vector<TensorInfo>& input_tensor_info_list);
    int Process(std::vector<TensorInfo>& output_tensor_info_list);
    void SetDlaCore(int dla_core) { dla_core_ = dla_core; }

   private:
    int AllocateBuffers(std::vector<TensorInfo>& input_tensor_info_list,
                        std::vector<TensorInfo>& output_tensor_info_list);

   private:
    int num_threads_;
    int dla_core_;
    std::unique_ptr<nvinfer1::IRuntime> runtime_;
    std::unique_ptr<nvinfer1::ICudaEngine> engine_;
    std::unique_ptr<nvinfer1::IExecutionContext> context_;
    std::vector<std::pair<void*, int>> buffer_list_cpu_;
    std::vector<std::pair<void*, int>> buffer_list_cpu_reserved_;
    std::vector<void*> buffer_list_gpu_;
};

}  // namespace glue
