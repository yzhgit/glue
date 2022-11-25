//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/nn/predictor.h"

namespace zdl {
namespace SNPE {
class SNPE;
}
}  // namespace zdl

namespace zdl {
namespace DlSystem {
class IUserBuffer;
}
}  // namespace zdl

namespace zdl {
namespace DlSystem {
class UserBufferMap;
}
}  // namespace zdl

namespace glue {

class PredictorImpl : public Predictor {
   private:
    enum { UNKNOWN, USERBUFFER_FLOAT, USERBUFFER_TF8, ITENSOR, USERBUFFER_TF16 };
    enum { CPUBUFFER, GLBUFFER };

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
    std::unique_ptr<zdl::SNPE::SNPE> CreateSnpe(const std::string& model_filename,
                                                bool use_user_supplied_buffers);
    int GetTensorInfo(std::unique_ptr<zdl::SNPE::SNPE> const& snpe,
                      const std::string& name,
                      std::vector<int>& dims);
    int GetAllTensorInfo(std::unique_ptr<zdl::SNPE::SNPE> const& snpe,
                         std::vector<TensorInfo>& input_tensor_info_list,
                         std::vector<TensorInfo>& output_tensor_info_list);

   private:
    int num_threads_;
    std::unique_ptr<zdl::SNPE::SNPE> snpe_;
    std::unique_ptr<zdl::DlSystem::UserBufferMap> input_map_;
    std::unique_ptr<zdl::DlSystem::UserBufferMap> output_map_;
    std::vector<std::unique_ptr<zdl::DlSystem::IUserBuffer>> snpe_user_input_buffers_;
    std::vector<std::unique_ptr<zdl::DlSystem::IUserBuffer>> snpe_user_output_buffers_;
    std::unordered_map<std::string, std::vector<uint8_t>> application_input_buffers_;
    std::unordered_map<std::string, std::vector<uint8_t>> application_output_buffers_;
};

}  // namespace glue
