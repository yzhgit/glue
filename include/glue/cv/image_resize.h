//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <math.h>
#include <stdint.h>

#include "glue/cv/image_preprocess.h"

namespace glue {

class ImageResize {
   public:
    void choose(const uint8_t* src,
                uint8_t* dst,
                ImageFormat srcFormat,
                int srcw,
                int srch,
                int dstw,
                int dsth);
};

void resize(const uint8_t* src,
            uint8_t* dst,
            ImageFormat srcFormat,
            int srcw,
            int srch,
            int dstw,
            int dsth);

}  // namespace glue
