#ifndef __PY_IMAGE_H
#define __PY_IMAGE_H

#include <stdint.h>
#include "imglib/imlib.h"

// typedef enum 
// {
//     PIXFORMAT_INVALID = 0,
//     PIXFORMAT_BINARY,    // 1BPP/BINARY
//     PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
//     PIXFORMAT_RGB565,    // 2BPP/RGB565
//     PIXFORMAT_YUV422,    // 2BPP/YUV422
//     PIXFORMAT_BAYER,     // 1BPP/RAW
//     PIXFORMAT_RGB888_HWC,
//     PIXFORMAT_RGB888_CHW,
//     PIXFORMAT_JPEG,      // JPEG/COMPRESSED
// } pixformat_t;

// typedef struct image 
// {
//     int w;
//     int h;
//     int bpp;
//     union 
//     {
//         uint8_t *pixels;
//         uint8_t *data;
//     };
//     void *ai;
// } image_t;

mp_obj_t py_img(int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels);
void *py_img_cobj(mp_obj_t img_obj);

#endif
