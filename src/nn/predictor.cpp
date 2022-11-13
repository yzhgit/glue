//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#if defined(GLUE_USE_NCNN)
    #include "ncnn/predictor_impl.h"
#elif defined(GLUE_USE_SNPE)
    #include "snpe/predictor_impl.h"
#elif defined(GLUE_USE_TENSORRT)
    #include "tensorrt/predictor_impl.h"
#endif

namespace glue {

Predictor::Predictor() : m_impl(new PredictorImpl)
{}

Predictor::~Predictor()
{}

int Predictor::SetNumThreads(int num_threads)
{
    return m_impl->SetNumThreads(num_threads);
}

int Predictor::Initialize(const std::string& model_filename,
                          std::vector<TensorInfo>& input_tensor_info_list,
                          std::vector<TensorInfo>& output_tensor_info_list)
{
    return m_impl->Initialize(model_filename, input_tensor_info_list, output_tensor_info_list);
}

int Predictor::Finalize(void)
{
    return m_impl->Finalize();
}

int Predictor::PreProcess(const std::vector<TensorInfo>& input_tensor_info_list)
{
    return m_impl->PreProcess(input_tensor_info_list);
}

int Predictor::Process(std::vector<TensorInfo>& output_tensor_info_list)
{
    return m_impl->Process(output_tensor_info_list);
}

} // namespace glue
