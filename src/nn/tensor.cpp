//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/nn/tensor.h"

GLUE_START_NAMESPACE
namespace ml {

constexpr uint32_t GetDataTypeSize(DataType dataType) {
    switch (dataType) {
    case DataType::BFloat16:
    case DataType::Float16:
        return 2U;
    case DataType::Float32:
    case DataType::Signed32:
        return 4U;
    case DataType::Signed64:
        return 8U;
    case DataType::QAsymmU8:
        return 1U;
    case DataType::QAsymmS8:
        return 1U;
    case DataType::QSymmS8:
        return 1U;
    case DataType::QSymmS16:
        return 2U;
    case DataType::Boolean:
        return 1U;
    default:
        return 0U;
    }
}

uint32_t TensorInfo::GetNumDimensions() const { return m_Shape.size(); }

uint32_t TensorInfo::GetNumElements() const {
    uint32_t elementNum = 1;
    for (const auto &dim : m_Shape) {
        elementNum *= dim;
    }
    return elementNum;
}

uint32_t TensorInfo::GetNumBytes() const {
    return GetNumElements() * GetDataTypeSize(m_DataType);
}

} // namespace ml
GLUE_END_NAMESPACE
