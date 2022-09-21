//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "jigger/cv/image_preprocess.h"

#include <math.h>
#include <stdint.h>

namespace glue {
namespace ml {

class ImageResize {
  public:
    void choose(const uint8_t *src, uint8_t *dst, ImageFormat srcFormat,
                int srcw, int srch, int dstw, int dsth);
};

void resize(const uint8_t *src, uint8_t *dst, ImageFormat srcFormat, int srcw,
            int srch, int dstw, int dsth);

} // namespace ml
} // namespace glue
