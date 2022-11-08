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
#include <unordered_map>
#include <vector>

#include "glue/nn/predictor.h"

namespace zdl {
namespace SNPE {
class SNPE;
}
} // namespace zdl

namespace zdl {
namespace DlSystem {
class IUserBuffer;
}
} // namespace zdl

namespace zdl {
namespace DlSystem {
class UserBufferMap;
}
} // namespace zdl

GLUE_START_NAMESPACE


class PredictorSnpe : public Predictor {
  private:
    enum {
        UNKNOWN,
        USERBUFFER_FLOAT,
        USERBUFFER_TF8,
        ITENSOR,
        USERBUFFER_TF16
    };
    enum { CPUBUFFER, GLBUFFER };

  public:
    PredictorSnpe();
    ~PredictorSnpe() override;
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
    std::unique_ptr<zdl::SNPE::SNPE>
    CreateSnpe(const std::string &model_filename,
               bool use_user_supplied_buffers);
    int32_t GetTensorInfo(std::unique_ptr<zdl::SNPE::SNPE> const &snpe,
                          const std::string &name, std::vector<int32_t> &dims);
    int32_t
    GetAllTensorInfo(std::unique_ptr<zdl::SNPE::SNPE> const &snpe,
                     std::vector<InputTensorInfo> &input_tensor_info_list,
                     std::vector<OutputTensorInfo> &output_tensor_info_list);

  private:
    int32_t num_threads_;
    std::unique_ptr<zdl::SNPE::SNPE> snpe_;
    std::unique_ptr<zdl::DlSystem::UserBufferMap> input_map_;
    std::unique_ptr<zdl::DlSystem::UserBufferMap> output_map_;
    std::vector<std::unique_ptr<zdl::DlSystem::IUserBuffer>>
        snpe_user_input_buffers_;
    std::vector<std::unique_ptr<zdl::DlSystem::IUserBuffer>>
        snpe_user_output_buffers_;
    std::unordered_map<std::string, std::vector<uint8_t>>
        application_input_buffers_;
    std::unordered_map<std::string, std::vector<uint8_t>>
        application_output_buffers_;
};


GLUE_END_NAMESPACE
