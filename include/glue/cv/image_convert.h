//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <stdint.h>
#include <stdio.h>

#include "glue/cv/image_preprocess.h"

namespace glue {

typedef void (*convert_func)(const uint8_t* src, uint8_t* dst, int srcw, int srch);

class ImageConvert {
   public:
    void choose(const uint8_t* src,
                uint8_t* dst,
                ImageFormat srcFormat,
                ImageFormat dstFormat,
                int srcw,
                int srch);

   private:
    convert_func impl_{nullptr};
};

}  // namespace glue
