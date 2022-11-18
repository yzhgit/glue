//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.h"

namespace glue {

// Returns the size of the output. If called with out = NULL, will just return
// the size of what the output would have been (without a terminating NULL).
GLUE_API size_t base64_encode(const uint8_t in[], uint8_t out[], size_t len, int newline_flag);

// Returns the size of the output. If called with out = NULL, will just return
// the size of what the output would have been (without a terminating NULL).
GLUE_API size_t base64_decode(const uint8_t in[], uint8_t out[], size_t len);

} // namespace glue
