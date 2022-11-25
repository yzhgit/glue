//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <vector>

#include "glue/nn/tensor.h"

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

class ImagePreprocess {
   public:
    /*
     * init
     * param srcFormat: input image color
     * param dstFormat: output image color
     * param param: input image parameter, egs: input size
     */
    ImagePreprocess(ImageFormat srcFormat, ImageFormat dstFormat, TransParam param);

    /*
     * image color convert
     * support NV12/NV21_to_BGR(RGB), NV12/NV21_to_BGRA(RGBA),
     * BGR(RGB)and BGRA(RGBA) transform,
     * BGR(RGB)and RGB(BGR) transform,
     * BGR(RGB)and RGBA(BGRA) transform,
     * BGR(RGB) and GRAY transform,
     * BGRA(RGBA) and GRAY transform,
     * param src: input image data
     * param dst: output image data
     */
    void image_convert(const uint8_t* src, uint8_t* dst);

    /*
     * image color convert
     * support NV12/NV21_to_BGR(RGB), NV12/NV21_to_BGRA(RGBA),
     * BGR(RGB)and BGRA(RGBA) transform,
     * BGR(RGB)and RGB(BGR) transform,
     * BGR(RGB)and RGBA(BGRA) transform,
     * BGR(RGB)and GRAY transform,
     * BGRA(RGBA) and GRAY transform,
     * param src: input image data
     * param dst: output image data
     * param srcFormat: input image image format support: GRAY, NV12(NV21),
     * BGR(RGB) and BGRA(RGBA)
     * param dstFormat: output image image format, support GRAY, BGR(RGB) and
     * BGRA(RGBA)
     */
    void image_convert(const uint8_t* src,
                       uint8_t* dst,
                       ImageFormat srcFormat,
                       ImageFormat dstFormat);

    /*
     * image color convert
     * support NV12/NV21_to_BGR(RGB), NV12/NV21_to_BGRA(RGBA),
     * BGR(RGB)and BGRA(RGBA) transform,
     * BGR(RGB)and RGB(BGR) transform,
     * BGR(RGB)and RGBA(BGRA) transform,
     * BGR(RGB)and GRAY transform,
     * BGRA(RGBA) and GRAY transform,
     * param src: input image data
     * param dst: output image data
     * param srcFormat: input image image format support: GRAY, NV12(NV21),
     * BGR(RGB) and BGRA(RGBA)
     * param dstFormat: output image image format, support GRAY, BGR(RGB) and
     * BGRA(RGBA)
     * param srcw: input image width
     * param srch: input image height
     */
    void image_convert(const uint8_t* src,
                       uint8_t* dst,
                       ImageFormat srcFormat,
                       ImageFormat dstFormat,
                       int srcw,
                       int srch);

    /*
     * image resize, use bilinear method
     * support image format: 1-channel image (egs: GRAY, 2-channel image (egs:
     * NV12, NV21), 3-channel(egs: BGR), 4-channel(egs: BGRA)
     * param src: input image data
     * param dst: output image data
     */
    void image_resize(const uint8_t* src, uint8_t* dst);

    /*
     image resize, use bilinear method
    * support image format: 1-channel image (egs: GRAY, 2-channel image (egs:
    NV12, NV21), 3-channel image(egs: BGR), 4-channel image(egs: BGRA)
    * param src: input image data
    * param dst: output image data
    * param srcw: input image width
    * param srch: input image height
    * param dstw: output image width
    * param dsth: output image height
    */
    void image_resize(const uint8_t* src,
                      uint8_t* dst,
                      ImageFormat srcFormat,
                      int srcw,
                      int srch,
                      int dstw,
                      int dsth);

    /*
     * image Rotate
     * support 90, 180 and 270 Rotate process
     * color format support 1-channel image, 3-channel image and 4-channel image
     * param src: input image data
     * param dst: output image data
     */
    void image_rotate(const uint8_t* src, uint8_t* dst);

    /*
     * image Rotate
     * support 90, 180 and 270 Rotate process
     * color format support 1-channel image, 3-channel image and 4-channel image
     * param src: input image data
     * param dst: output image data
     * param srcFormat: input image format, support GRAY, BGR(RGB) and
     * BGRA(RGBA) param srcw: input image width param srch: input image height
     * param degree: Rotate degree, support 90, 180 and 270
     */
    void image_rotate(
        const uint8_t* src, uint8_t* dst, ImageFormat srcFormat, int srcw, int srch, float degree);

    /*
     * image Flip
     * support X, Y and XY flip process
     * color format support 1-channel image, 3-channel image and 4-channel image
     * param src: input image data
     * param dst: output image data
     */
    void image_flip(const uint8_t* src, uint8_t* dst);

    /*
     * image Flip
     * support X, Y and XY flip process
     * color format support 1-channel image, 3-channel image and 4-channel image
     * param src: input image data
     * param dst: output image data
     * param srcFormat: input image format, support GRAY, BGR(RGB) and
     * BGRA(RGBA) param srcw: input image width param srch: input image height
     * param flip_param: flip parameter, support X, Y and XY
     */
    void image_flip(const uint8_t* src,
                    uint8_t* dst,
                    ImageFormat srcFormat,
                    int srcw,
                    int srch,
                    FlipParam flip_param);

    /*
     * change image data to tensor data
     * support image format is GRAY, BGR(RGB) and BGRA(RGBA), Data layout is
     * NHWC and NCHW param src: input image data param dstTensor: output tensor
     * data param layout: output tensor layout，support NHWC and NCHW param
     * means: means of image param scales: scales of image
     */
    void image_to_tensor(
        const uint8_t* src, Tensor* dstTensor, LayoutType layout, float* means, float* scales);

    /*
     * change image data to tensor data
     * support image format is GRAY, BGR(RGB) and BGRA(RGBA), Data layout is
     * NHWC and NCHW param src: input image data param dstTensor: output tensor
     * data param srcFormat: input image format, support BGR(RGB) and BGRA(RGBA)
     * param srcw: input image width
     * param srch: input image height
     * param layout: output tensor layout，support NHWC and NCHW
     * param means: means of image
     * param scales: scales of image
     */
    void image_to_tensor(const uint8_t* src,
                         Tensor* dstTensor,
                         ImageFormat srcFormat,
                         int srcw,
                         int srch,
                         LayoutType layout,
                         float* means,
                         float* scales);

    /*
     * image crop process
     * color format support 1-channel image, 3-channel image and 4-channel image
     * param src: input image data
     * param dst: output image data
     */
    void image_crop(const uint8_t* src,
                    uint8_t* dst,
                    ImageFormat srcFormat,
                    int srcw,
                    int srch,
                    int left_x,
                    int left_y,
                    int dstw,
                    int dsth);

   private:
    ImageFormat srcFormat_;
    ImageFormat dstFormat_;
    TransParam transParam_;
};

}  // namespace glue
