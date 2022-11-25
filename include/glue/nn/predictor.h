//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "glue/nn/tensor.h"

namespace glue {

class PredictorImpl;

class Predictor {
   public:
    Predictor();
    ~Predictor();

    /**
     * @description: create engine from rknn model
     * @onnxModel: path to rknn model
     * @return:
     */
    bool Initialize(const char* model_filename, const char* model_data, size_t model_length);

    /**
     * @description: do inference on engine context, make sure you already copy
     * your data to device memory, see CopyData etc.
     */
    std::vector<Tensor> Forward(std::vector<Tensor>& inputs);

    //!
    //! \brief Get the number of binding indices.
    //!
    //! \see GetBindingIndex();
    //!
    int GetNbBindings() const;

    //!
    //! \brief Determine whether a binding is an input binding.
    //!
    //! \param bindIndex The binding index.
    //! \return True if the index corresponds to an input binding and the index
    //! is in range.
    //!
    //! \see GetBindingIndex()
    //!
    bool BindingIsInput(int bindIndex) const;

    //!
    //! \brief Retrieve the binding index for a named tensor.
    //!
    //!
    //! Engine bindings map from tensor names to indices in this array.
    //! Binding indices are assigned at engine build time, and take values in
    //! the range [0 ... n-1] where n is the total number of inputs and outputs.
    //!
    //! \param name The tensor name.
    //! \return The binding index for the named tensor, or -1 if the name is not
    //! found.
    //!
    //! see GetNbBindings() GetBindingIndex()
    //!
    int GetBindingIndex(const char* name) const;

    //!
    //! \brief Retrieve the name corresponding to a binding index.
    //!
    //! This is the reverse mapping to that provided by GetBindingIndex().
    //!
    //! \param bindIndex The binding index.
    //! \return The name corresponding to the index, or NULL if the index is out
    //! of range.
    //!
    //! \see GetBindingIndex()
    //!
    const char* GetBindingName(int bindIndex) const;

    /**
     * @description: get binding data size in byte, so maybe you need to divide
     * it by sizeof(T) where T is data type like float.
     * @return: size in byte.
     */
    size_t GetBindingSize(int bindIndex) const;

    /**
     * @description: get binding dimemsions
     * @return: binding dimemsions
     */
    std::vector<int> GetBindingDims(int bindIndex) const;

    /**
     * @description: get binding data type
     * @return: binding data type
     */
    DataType GetBindingDataType(int bindIndex) const;

   private:
    std::unique_ptr<PredictorImpl> m_impl;
};

}  // namespace glue
