//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

namespace glue {
namespace ml {

enum class DataType {
    Float16 = 0,
    Float32 = 1,
    QAsymmU8 = 2,
    Signed32 = 3,
    Boolean = 4,
    QSymmS16 = 5,
    QSymmS8 = 6,
    QAsymmS8 = 7,
    BFloat16 = 8,
    Signed64 = 9,
};

enum class DataLayout { NCHW, NHWC};

enum class QuantType { NONE, ASYMMETRIC, SYMMETRIC_PER_CHANNEL };

enum class PadType { NONE = -1, AUTO, VALID, SAME };

enum class PoolType { MAX, AVG, L2, AVG_ANDROID };

enum class RoundType { CEILING, FLOOR };

enum class OverflowPolicy { WRAP, SATURATE };

enum class RoundingPolicy { TO_ZERO, RTNE };

enum class ResizeType { NEAREST_NEIGHBOR, BILINEAR, AREA };

} // namespace ml
GLUE_END_NAMESPACE
