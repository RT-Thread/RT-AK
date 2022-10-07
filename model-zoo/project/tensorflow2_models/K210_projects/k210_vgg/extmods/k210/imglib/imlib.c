/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image library.
 */
#include <stdlib.h>
#include "font.h"
#include "imlib.h"
#include "common.h"
/* origin openmv @file:imlib.c @file:38 */
/////////////////
// Point Stuff //
/////////////////

void point_init(point_t *ptr, int x, int y)
{
    ptr->x = x;
    ptr->y = y;
}

void point_copy(point_t *dst, point_t *src)
{
    memcpy(dst, src, sizeof(point_t));
}

bool point_equal_fast(point_t *ptr0, point_t *ptr1)
{
    return !memcmp(ptr0, ptr1, sizeof(point_t));
}

int point_quadrance(point_t *ptr0, point_t *ptr1)
{
    int delta_x = ptr0->x - ptr1->x;
    int delta_y = ptr0->y - ptr1->y;
    return (delta_x * delta_x) + (delta_y * delta_y);
}

void point_rotate(int x, int y, float r, int center_x, int center_y, int16_t *new_x, int16_t *new_y)
{
    x -= center_x;
    y -= center_y;
    *new_x = (x * cosf(r)) - (y * sinf(r)) + center_x;
    *new_y = (x * sinf(r)) + (y * cosf(r)) + center_y;
}

void point_min_area_rectangle(point_t *corners, point_t *new_corners, int corners_len) // Corners need to be sorted!
{
    int i_min = 0;
    int i_min_area = INT_MAX;
    int i_x0 = 0, i_y0 = 0;
    int i_x1 = 0, i_y1 = 0;
    int i_x2 = 0, i_y2 = 0;
    int i_x3 = 0, i_y3 = 0;
    float i_r = 0;

    // This algorithm aligns the 4 edges produced by the 4 corners to the x axis and then computes the
    // min area rect for each alignment. The smallest rect is choosen and then re-rotated and returned.
    for (int i = 0; i < corners_len; i++) {
        int16_t x0 = corners[i].x, y0 = corners[i].y;
        int x_diff = corners[(i+1)%corners_len].x - corners[i].x;
        int y_diff = corners[(i+1)%corners_len].y - corners[i].y;
        float r = -fast_atan2f(y_diff, x_diff);

        int16_t x1[corners_len-1];
        int16_t y1[corners_len-1];
        for (int j = 0, jj = corners_len - 1; j < jj; j++) {
            point_rotate(corners[(i+j+1)%corners_len].x, corners[(i+j+1)%corners_len].y, r, x0, y0, x1 + j, y1 + j);
        }

        int minx = x0;
        int maxx = x0;
        int miny = y0;
        int maxy = y0;
        for (int j = 0, jj = corners_len - 1; j < jj; j++) {
            minx = IM_MIN(minx, x1[j]);
            maxx = IM_MAX(maxx, x1[j]);
            miny = IM_MIN(miny, y1[j]);
            maxy = IM_MAX(maxy, y1[j]);
        }

        int area = (maxx - minx + 1) * (maxy - miny + 1);
        if (area < i_min_area) {
            i_min = i;
            i_min_area = area;
            i_x0 = minx, i_y0 = miny;
            i_x1 = maxx, i_y1 = miny;
            i_x2 = maxx, i_y2 = maxy;
            i_x3 = minx, i_y3 = maxy;
            i_r = r;
        }
    }

    point_rotate(i_x0, i_y0, -i_r, corners[i_min].x, corners[i_min].y, &new_corners[0].x, &new_corners[0].y);
    point_rotate(i_x1, i_y1, -i_r, corners[i_min].x, corners[i_min].y, &new_corners[1].x, &new_corners[1].y);
    point_rotate(i_x2, i_y2, -i_r, corners[i_min].x, corners[i_min].y, &new_corners[2].x, &new_corners[2].y);
    point_rotate(i_x3, i_y3, -i_r, corners[i_min].x, corners[i_min].y, &new_corners[3].x, &new_corners[3].y);
}

/* origin openmv @file:imlib.c @line:239 */
/////////////////
// Image Stuff //
/////////////////
void image_init(image_t *ptr, int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels)
{
    ptr->w = w;
    ptr->h = h;
    ptr->pixfmt = pixfmt;
    ptr->size   = size;
    ptr->pixels = pixels;
}

void image_copy(image_t *dst, image_t *src)
{
    memcpy(dst, src, sizeof(image_t));
}

size_t image_size(image_t *ptr)
{
    switch (ptr->pixfmt) {
        case PIXFORMAT_BINARY: {
            return IMAGE_BINARY_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        case PIXFORMAT_GRAYSCALE:
        case PIXFORMAT_BAYER_ANY: { // re-use
            return IMAGE_GRAYSCALE_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        case PIXFORMAT_RGB565:
        case PIXFORMAT_YUV_ANY: { // re-use
            return IMAGE_RGB565_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        case PIXFORMAT_JPEG: {
            return ptr->size;
        }
        case PIXFORMAT_RGB888_CHW:
        case PIXFORMAT_RGB888_HWC:
        {
            return IMAGE_RGB888_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        default: {
            return 0;
        }
    }
}
/* end openmv @file:imlib.c @line:278 */
/* start openmv @file:imlib.c @line:448 */
// https://en.wikipedia.org/wiki/Lab_color_space -> CIELAB-CIEXYZ conversions
// https://en.wikipedia.org/wiki/SRGB -> Specification of the transformation
uint16_t imlib_lab_to_rgb(uint8_t l, int8_t a, int8_t b)
{
    float x = ((l + 16) * 0.008621f) + (a * 0.002f);
    float y = ((l + 16) * 0.008621f);
    float z = ((l + 16) * 0.008621f) - (b * 0.005f);

    x = ((x > 0.206897f) ? (x*x*x) : ((0.128419f * x) - 0.017713f)) * 095.047f;
    y = ((y > 0.206897f) ? (y*y*y) : ((0.128419f * y) - 0.017713f)) * 100.000f;
    z = ((z > 0.206897f) ? (z*z*z) : ((0.128419f * z) - 0.017713f)) * 108.883f;

    float r_lin = ((x * +3.2406f) + (y * -1.5372f) + (z * -0.4986f)) / 100.0f;
    float g_lin = ((x * -0.9689f) + (y * +1.8758f) + (z * +0.0415f)) / 100.0f;
    float b_lin = ((x * +0.0557f) + (y * -0.2040f) + (z * +1.0570f)) / 100.0f;

    r_lin = (r_lin>0.0031308f) ? ((1.055f*powf(r_lin, 0.416666f))-0.055f) : (r_lin*12.92f);
    g_lin = (g_lin>0.0031308f) ? ((1.055f*powf(g_lin, 0.416666f))-0.055f) : (g_lin*12.92f);
    b_lin = (b_lin>0.0031308f) ? ((1.055f*powf(b_lin, 0.416666f))-0.055f) : (b_lin*12.92f);

    uint32_t red   = IM_MAX(IM_MIN(fast_floorf(r_lin * COLOR_R8_MAX), COLOR_R8_MAX), COLOR_R8_MIN);
    uint32_t green = IM_MAX(IM_MIN(fast_floorf(g_lin * COLOR_G8_MAX), COLOR_G8_MAX), COLOR_G8_MIN);
    uint32_t blue  = IM_MAX(IM_MIN(fast_floorf(b_lin * COLOR_B8_MAX), COLOR_B8_MAX), COLOR_B8_MIN);

    return COLOR_R8_G8_B8_TO_RGB565(red, green, blue);
}

// https://en.wikipedia.org/wiki/YCbCr -> JPEG Conversion
uint16_t imlib_yuv_to_rgb(uint8_t y, int8_t u, int8_t v)
{
    uint32_t r = IM_MAX(IM_MIN(y + ((91881 * v) >> 16), COLOR_R8_MAX), COLOR_R8_MIN);
    uint32_t g = IM_MAX(IM_MIN(y - (((22554 * u) + (46802 * v)) >> 16), COLOR_G8_MAX), COLOR_G8_MIN);
    uint32_t b = IM_MAX(IM_MIN(y + ((116130 * u) >> 16), COLOR_B8_MAX), COLOR_B8_MIN);

    return COLOR_R8_G8_B8_TO_RGB565(r, g, b);
}
/* end openmv @file:imlib.c @line:484*/