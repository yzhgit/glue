//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <stdint.h>

namespace glue {

void bgr_rotate_hwc(const uint8_t* src, uint8_t* dst, int w_in, int h_in, int angle);

}
