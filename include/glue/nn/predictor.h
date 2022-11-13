//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace glue {

class PredictorImpl;

class Predictor
{
public:
    Predictor();
    ~Predictor();
    int32_t SetNumThreads(const int32_t num_threads);
    int32_t Initialize(const std::string& model_filename,
                       std::vector<InputTensorInfo>& input_tensor_info_list,
                       std::vector<OutputTensorInfo>& output_tensor_info_list);
    int32_t Finalize(void);
    int32_t PreProcess(const std::vector<InputTensorInfo>& input_tensor_info_list);
    int32_t Process(std::vector<OutputTensorInfo>& output_tensor_info_list);

protected:
    void ConvertNormalizeParameters(InputTensorInfo& tensor_info);

    void PreProcessImage(int32_t num_thread, const InputTensorInfo& input_tensor_info, float* dst);
    void PreProcessImage(int32_t num_thread, const InputTensorInfo& input_tensor_info,
                         uint8_t* dst);
    void PreProcessImage(int32_t num_thread, const InputTensorInfo& input_tensor_info, int8_t* dst);

    template <typename T>
    void PreProcessBlob(int32_t num_thread, const InputTensorInfo& input_tensor_info, T* dst);

private:
    std::unique_ptr<PredictorImpl> m_impl;
};

} // namespace glue
