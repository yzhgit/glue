//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <stdint.h>
#include <vector>

#include "glue/cv/image_preprocess.h"

namespace glue {

class ImageFlip {
   public:
    void choose(const uint8_t* src,
                uint8_t* dst,
                ImageFormat srcFormat,
                int srcw,
                int srch,
                FlipParam flip_param);
};

void flip_hwc1(const uint8_t* src, uint8_t* dst, int srcw, int srch, FlipParam flip_param);
void flip_hwc3(const uint8_t* src, uint8_t* dst, int srcw, int srch, FlipParam flip_param);
void flip_hwc4(const uint8_t* src, uint8_t* dst, int srcw, int srch, FlipParam flip_param);

}  // namespace glue
