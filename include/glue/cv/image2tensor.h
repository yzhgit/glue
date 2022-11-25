//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <stdint.h>

#include "glue/cv/image_preprocess.h"

namespace glue {

typedef void (*tensor_func)(
    const uint8_t* src, float* dst, int srcw, int srch, float* means, float* scales);

class Image2Tensor {
   public:
    void choose(const uint8_t* src,
                Tensor* dst,
                ImageFormat srcFormat,
                DataLayout layout,
                int srcw,
                int srch,
                float* means,
                float* scales);

   private:
    tensor_func impl_{nullptr};
};

}  // namespace glue
