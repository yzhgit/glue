//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <iostream>

namespace glue {
namespace ml {

#if defined(__clang__) &&                                                      \
    ((__clang_major__ >= 3) || (__clang_major__ == 3 && __clang_minor__ >= 5))
    #define ARMNN_FALLTHROUGH [[clang::fallthrough]]
#elif defined(__GNUC__) && (__GNUC__ >= 7)
    #define ARMNN_FALLTHROUGH __attribute__((fallthrough))
#else
    #define ARMNN_FALLTHROUGH ((void)0)
#endif

bool NeonDetected();

} // namespace ml
} // namespace glue
