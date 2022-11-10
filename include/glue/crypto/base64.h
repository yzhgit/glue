//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

namespace glue {

typedef unsigned char BYTE; // 8-bit byte

// Returns the size of the output. If called with out = NULL, will just return
// the size of what the output would have been (without a terminating NULL).
GLUE_API size_t base64_encode(const BYTE in[], BYTE out[], size_t len, int newline_flag);

// Returns the size of the output. If called with out = NULL, will just return
// the size of what the output would have been (without a terminating NULL).
GLUE_API size_t base64_decode(const BYTE in[], BYTE out[], size_t len);

} // namespace glue
