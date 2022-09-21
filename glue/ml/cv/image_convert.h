//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "jigger/cv/image_preprocess.h"

#include <stdint.h>
#include <stdio.h>

namespace glue {
namespace ml {

typedef void (*convert_func)(const uint8_t *src, uint8_t *dst, int srcw,
                             int srch);

class ImageConvert {
  public:
    void choose(const uint8_t *src, uint8_t *dst, ImageFormat srcFormat,
                ImageFormat dstFormat, int srcw, int srch);

  private:
    convert_func impl_{nullptr};
};

} // namespace ml
} // namespace glue
