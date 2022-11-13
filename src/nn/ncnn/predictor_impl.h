//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/nn/predictor.h"

#include "net.h"

namespace glue {

class PredictorImpl : public Predictor
{
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

private:
    int num_threads_;
    std::unique_ptr<ncnn::Net> net_;
    std::vector<std::pair<std::string, ncnn::Mat>> in_mat_list_;
    std::vector<ncnn::Mat> out_mat_list_;
};

} // namespace glue
