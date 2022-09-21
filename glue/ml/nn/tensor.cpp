//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "nn/tensor.h"

namespace glue {
namespace ml {

constexpr unsigned int GetDataTypeSize(DataType dataType) {
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

} // namespace ml
} // namespace glue
