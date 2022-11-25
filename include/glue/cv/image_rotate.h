//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/cv/types.h"

namespace glue {

class ImageRotate {
   public:
    void choose(
        const uint8_t* src, uint8_t* dst, ImageFormat srcFormat, int srcw, int srch, float degree);
};

void rotate_hwc1(const uint8_t* src, uint8_t* dst, int srcw, int srch, float degree);
void rotate_hwc3(const uint8_t* src, uint8_t* dst, int srcw, int srch, float degree);
void rotate_hwc4(const uint8_t* src, uint8_t* dst, int srcw, int srch, float degree);

}  // namespace glue
