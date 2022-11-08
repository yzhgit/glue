//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/cv/image_preprocess.h"

#include <math.h>
#include <stdint.h>

GLUE_START_NAMESPACE

class ImageResize
{
public:
    void choose(const uint8_t* src, uint8_t* dst, ImageFormat srcFormat, int srcw, int srch,
                int dstw, int dsth);
};

void resize(const uint8_t* src, uint8_t* dst, ImageFormat srcFormat, int srcw, int srch, int dstw,
            int dsth);

GLUE_END_NAMESPACE
