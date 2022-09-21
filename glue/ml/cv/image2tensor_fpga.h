//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "jigger/cv/image_preprocess.h"
#include "lite/backends/fpga/KD/float16.hpp"
#include "lite/backends/fpga/KD/llapi/zynqmp_api.h"
#include <stdint.h>

namespace glue {
namespace ml {

using paddle::zynqmp::float16;

typedef void (*tensor_func_fpga)(const uint8_t *src, float16 *output,
                                 ImageFormat srcFormat, ImageFormat dstFormat,
                                 int srcw, int srch, int dstw, int dsth,
                                 float *means, float *scales);

class Image2TensorFpga {
  public:
    void choose(const uint8_t *src, Tensor *dst, ImageFormat srcFormat,
                ImageFormat dstFormat, LayoutType layout, int srcw, int srch,
                int dstw, int dsth, float *means, float *scales);

  private:
    tensor_func_fpga impl_{nullptr};
};

} // namespace ml
} // namespace glue
