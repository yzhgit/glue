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

#include "net.h"

#include "glue/nn/predictor.h"

GLUE_START_NAMESPACE
namespace ml {

class PredictorNcnn : public Predictor {
  public:
    PredictorNcnn();
    ~PredictorNcnn() override;
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

  private:
    std::unique_ptr<ncnn::Net> net_;
    std::vector<std::pair<std::string, ncnn::Mat>> in_mat_list_; // <name, mat>
    std::vector<ncnn::Mat> out_mat_list_;
    int32_t num_threads_;
};

} // namespace ml
GLUE_END_NAMESPACE
