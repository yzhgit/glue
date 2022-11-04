//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/cv/image_preprocess.h"

#include <stdint.h>
#include <vector>

GLUE_START_NAMESPACE
namespace ml {

class ImageFlip {
  public:
    void choose(const uint8_t *src, uint8_t *dst, ImageFormat srcFormat,
                int srcw, int srch, FlipParam flip_param);
};

void flip_hwc1(const uint8_t *src, uint8_t *dst, int srcw, int srch,
               FlipParam flip_param);
void flip_hwc3(const uint8_t *src, uint8_t *dst, int srcw, int srch,
               FlipParam flip_param);
void flip_hwc4(const uint8_t *src, uint8_t *dst, int srcw, int srch,
               FlipParam flip_param);

} // namespace ml
GLUE_END_NAMESPACE
