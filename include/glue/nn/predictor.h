//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "glue/nn/tensor.h"

namespace glue {

class PredictorImpl;

class Predictor
{
public:
    Predictor();
    ~Predictor();
    int SetNumThreads(int num_threads);
    int Initialize(const std::string& model_filename,
                   std::vector<TensorInfo>& input_tensor_info_list,
                   std::vector<TensorInfo>& output_tensor_info_list);
    int Finalize(void);
    int PreProcess(const std::vector<TensorInfo>& input_tensor_info_list);
    int Process(std::vector<TensorInfo>& output_tensor_info_list);

private:
    std::unique_ptr<PredictorImpl> m_impl;
};

} // namespace glue
