//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <stdint.h>

namespace glue {

// color enum
enum ImageFormat {
    RGBA = 0,
    BGRA,
    RGB,
    BGR,
    GRAY,
    NV21 = 11,
    NV12,
    YUV420SP,
    YUV420P,
    YUV422,
    YUV444
};

// flip enum
enum FlipParam {
    XY = -1,  // flip along the XY axis
    X = 0,    // flip along the X axis
    Y         // flip along the Y axis
};

// transform param
typedef struct {
    int ih;                // input height
    int iw;                // input width
    int oh;                // outpu theight
    int ow;                // output width
    FlipParam flip_param;  // flip, support x, y, xy
    float rotate_param;    // rotate, support 90, 180, 270
} TransParam;

}  // namespace glue
