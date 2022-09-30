/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image processing library.
 */
#ifndef __IMLIB_H__
#define __IMLIB_H__
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "fmath.h"

#include "__cmsis_gcc.h"
#include <imlib_config.h>
#include <imlib_exts.h>

/* start openmv @file:imglib.h @line:32 */
#ifndef M_PI
#define M_PI    3.14159265f
#define M_PI_2  1.57079632f
#define M_PI_4  0.78539816f
#endif

#define IM_LOG2_2(x)    (((x) &                0x2ULL) ? ( 2                        ) :             1) // NO ({ ... }) !
#define IM_LOG2_4(x)    (((x) &                0xCULL) ? ( 2 +  IM_LOG2_2((x) >>  2)) :  IM_LOG2_2(x)) // NO ({ ... }) !
#define IM_LOG2_8(x)    (((x) &               0xF0ULL) ? ( 4 +  IM_LOG2_4((x) >>  4)) :  IM_LOG2_4(x)) // NO ({ ... }) !
#define IM_LOG2_16(x)   (((x) &             0xFF00ULL) ? ( 8 +  IM_LOG2_8((x) >>  8)) :  IM_LOG2_8(x)) // NO ({ ... }) !
#define IM_LOG2_32(x)   (((x) &         0xFFFF0000ULL) ? (16 + IM_LOG2_16((x) >> 16)) : IM_LOG2_16(x)) // NO ({ ... }) !
#define IM_LOG2(x)      (((x) & 0xFFFFFFFF00000000ULL) ? (32 + IM_LOG2_32((x) >> 32)) : IM_LOG2_32(x)) // NO ({ ... }) !

#define IM_MAX(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define IM_MIN(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define IM_DIV(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a / _b) : 0; })
#define IM_MOD(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a % _b) : 0; })

#define INT8_T_BITS     (sizeof(int8_t) * 8)
#define INT8_T_MASK     (INT8_T_BITS - 1)
#define INT8_T_SHIFT    IM_LOG2(INT8_T_MASK)

#define INT16_T_BITS    (sizeof(int16_t) * 8)
#define INT16_T_MASK    (INT16_T_BITS - 1)
#define INT16_T_SHIFT   IM_LOG2(INT16_T_MASK)

#define INT32_T_BITS    (sizeof(int32_t) * 8)
#define INT32_T_MASK    (INT32_T_BITS - 1)
#define INT32_T_SHIFT   IM_LOG2(INT32_T_MASK)

#define INT64_T_BITS    (sizeof(int64_t) * 8)
#define INT64_T_MASK    (INT64_T_BITS - 1)
#define INT64_T_SHIFT   IM_LOG2(INT64_T_MASK)

#define UINT8_T_BITS    (sizeof(uint8_t) * 8)
#define UINT8_T_MASK    (UINT8_T_BITS - 1)
#define UINT8_T_SHIFT   IM_LOG2(UINT8_T_MASK)

#define UINT16_T_BITS   (sizeof(uint16_t) * 8)
#define UINT16_T_MASK   (UINT16_T_BITS - 1)
#define UINT16_T_SHIFT  IM_LOG2(UINT16_T_MASK)

#define UINT32_T_BITS   (sizeof(uint32_t) * 8)
#define UINT32_T_MASK   (UINT32_T_BITS - 1)
#define UINT32_T_SHIFT  IM_LOG2(UINT32_T_MASK)

#define UINT64_T_BITS   (sizeof(uint64_t) * 8)
#define UINT64_T_MASK   (UINT64_T_BITS - 1)
#define UINT64_T_SHIFT  IM_LOG2(UINT64_T_MASK)

#define IM_DEG2RAD(x)   (((x)*M_PI)/180)
#define IM_RAD2DEG(x)   (((x)*180)/M_PI)
/* end openmv @file:imglib.h @line:83*/
/////////////////
// Point Stuff //
/////////////////

typedef struct point {
    int16_t x;
    int16_t y;
} point_t;

void point_init(point_t *ptr, int x, int y);
void point_copy(point_t *dst, point_t *src);
bool point_equal_fast(point_t *ptr0, point_t *ptr1);
int point_quadrance(point_t *ptr0, point_t *ptr1);
void point_rotate(int x, int y, float r, int center_x, int center_y, int16_t *new_x, int16_t *new_y);
// void point_min_area_rectangle(point_t *corners, point_t *new_corners, int corners_len);

/* start openmv @file:imlib.h @line:199 */
#define COLOR_BINARY_MIN 0
#define COLOR_BINARY_MAX 1
#define COLOR_GRAYSCALE_BINARY_MIN 0x00
#define COLOR_GRAYSCALE_BINARY_MAX 0xFF
#define COLOR_RGB565_BINARY_MIN 0x0000
#define COLOR_RGB565_BINARY_MAX 0xFFFF

#define COLOR_GRAYSCALE_MIN 0
#define COLOR_GRAYSCALE_MAX 255

#define COLOR_R5_MIN 0
#define COLOR_R5_MAX 31
#define COLOR_G6_MIN 0
#define COLOR_G6_MAX 63
#define COLOR_B5_MIN 0
#define COLOR_B5_MAX 31

#define COLOR_R8_MIN 0
#define COLOR_R8_MAX 255
#define COLOR_G8_MIN 0
#define COLOR_G8_MAX 255
#define COLOR_B8_MIN 0
#define COLOR_B8_MAX 255

#define COLOR_L_MIN 0
#define COLOR_L_MAX 100
#define COLOR_A_MIN -128
#define COLOR_A_MAX 127
#define COLOR_B_MIN -128
#define COLOR_B_MAX 127

#define COLOR_Y_MIN 0
#define COLOR_Y_MAX 255
#define COLOR_U_MIN -128
#define COLOR_U_MAX 127
#define COLOR_V_MIN -128
#define COLOR_V_MAX 127

// RGB565 Stuff //

#define COLOR_RGB565_TO_R5(pixel) (((pixel) >> 11) & 0x1F)
#define COLOR_RGB565_TO_R8(pixel) \
({ \
    __typeof__ (pixel) __pixel = (pixel); \
    __pixel = (__pixel >> 8) & 0xF8; \
    __pixel | (__pixel >> 5); \
})

#define COLOR_RGB565_TO_G6(pixel) (((pixel) >> 5) & 0x3F)
#define COLOR_RGB565_TO_G8(pixel) \
({ \
    __typeof__ (pixel) __pixel = (pixel); \
    __pixel = (__pixel >> 3) & 0xFC; \
    __pixel | (__pixel >> 6); \
})

#define COLOR_RGB565_TO_B5(pixel) ((pixel) & 0x1F)
#define COLOR_RGB565_TO_B8(pixel) \
({ \
    __typeof__ (pixel) __pixel = (pixel); \
    __pixel = (__pixel << 3) & 0xF8; \
    __pixel | (__pixel >> 5); \
})

#define COLOR_R5_G6_B5_TO_RGB565(r5, g6, b5) (((r5) << 11) | ((g6) << 5) | (b5))
#define COLOR_R8_G8_B8_TO_RGB565(r8, g8, b8) ((((r8) & 0xF8) << 8) | (((g8) & 0xFC) << 3) | ((b8) >> 3))

#define COLOR_RGB888_TO_Y(r8, g8, b8) ((((r8) * 38) + ((g8) * 75) + ((b8) * 15)) >> 7) // 0.299R + 0.587G + 0.114B
#define COLOR_RGB565_TO_Y(rgb565) \
({ \
    __typeof__ (rgb565) __rgb565 = (rgb565); \
    int r = COLOR_RGB565_TO_R8(__rgb565); \
    int g = COLOR_RGB565_TO_G8(__rgb565); \
    int b = COLOR_RGB565_TO_B8(__rgb565); \
    COLOR_RGB888_TO_Y(r, g, b); \
})

#define COLOR_Y_TO_RGB888(pixel) ((pixel) * 0x010101)
#define COLOR_Y_TO_RGB565(pixel) \
({ \
    __typeof__ (pixel) __pixel = (pixel); \
    int __rb_pixel = (__pixel >> 3) & 0x1F; \
    (__rb_pixel * 0x0801) + ((__pixel << 3) & 0x7E0); \
})

#define COLOR_RGB888_TO_U(r8, g8, b8) ((((r8) * -21) - ((g8) * 43) + ((b8) * 64)) >> 7) // -0.168736R - 0.331264G + 0.5B
#define COLOR_RGB565_TO_U(rgb565) \
({ \
    __typeof__ (rgb565) __rgb565 = (rgb565); \
    int r = COLOR_RGB565_TO_R8(__rgb565); \
    int g = COLOR_RGB565_TO_G8(__rgb565); \
    int b = COLOR_RGB565_TO_B8(__rgb565); \
    COLOR_RGB888_TO_U(r, g, b); \
})

#define COLOR_RGB888_TO_V(r8, g8, b8) ((((r8) * 64) - ((g8) * 54) - ((b8) * 10)) >> 7) // 0.5R - 0.418688G - 0.081312B
#define COLOR_RGB565_TO_V(rgb565) \
({ \
    __typeof__ (rgb565) __rgb565 = (rgb565); \
    int r = COLOR_RGB565_TO_R8(__rgb565); \
    int g = COLOR_RGB565_TO_G8(__rgb565); \
    int b = COLOR_RGB565_TO_B8(__rgb565); \
    COLOR_RGB888_TO_V(r, g, b); \
})

extern const int8_t lab_table[196608/2];

#ifdef IMLIB_ENABLE_LAB_LUT
#define COLOR_RGB565_TO_L(pixel) lab_table[((pixel>>1) * 3) + 0]
#define COLOR_RGB565_TO_A(pixel) lab_table[((pixel>>1) * 3) + 1]
#define COLOR_RGB565_TO_B(pixel) lab_table[((pixel>>1) * 3) + 2]
#else
#define COLOR_RGB565_TO_L(pixel) imlib_rgb565_to_l(pixel)
#define COLOR_RGB565_TO_A(pixel) imlib_rgb565_to_a(pixel)
#define COLOR_RGB565_TO_B(pixel) imlib_rgb565_to_b(pixel)
#endif

#define COLOR_LAB_TO_RGB565(l, a, b) imlib_lab_to_rgb(l, a, b)
#define COLOR_YUV_TO_RGB565(y, u, v) imlib_yuv_to_rgb((y) + 128, u, v)

#define COLOR_BINARY_TO_GRAYSCALE(pixel) ((pixel) * COLOR_GRAYSCALE_MAX)
#define COLOR_BINARY_TO_RGB565(pixel) COLOR_YUV_TO_RGB565(((pixel) ? 127 : -128), 0, 0)
#define COLOR_RGB565_TO_BINARY(pixel) (COLOR_RGB565_TO_Y(pixel) > (((COLOR_Y_MAX - COLOR_Y_MIN) / 2) + COLOR_Y_MIN))
#define COLOR_RGB565_TO_GRAYSCALE(pixel) COLOR_RGB565_TO_Y(pixel)
#define COLOR_GRAYSCALE_TO_BINARY(pixel) ((pixel) > (((COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN) / 2) + COLOR_GRAYSCALE_MIN))
#define COLOR_GRAYSCALE_TO_RGB565(pixel) COLOR_YUV_TO_RGB565(((pixel) - 128), 0, 0)
/* end openmv @file:imlib.h @line:324*/
/* start openmv @file:imlib.h @line:335 */
/////////////////
// Image Stuff //
/////////////////

// Pixel format IDs.
typedef enum {
    PIXFORMAT_ID_BINARY     = 1,
    PIXFORMAT_ID_GRAY       = 2,
    PIXFORMAT_ID_RGB565     = 3,
    PIXFORMAT_ID_BAYER      = 4,
    PIXFORMAT_ID_YUV422     = 5,
    PIXFORMAT_ID_JPEG       = 6,
    /* Note: Update PIXFORMAT_IS_VALID when adding new formats */
    PIXFORMAT_ID_RGB888     = 9,    // for rtak
} pixformat_id_t;

// Pixel sub-format IDs.
typedef enum {
    SUBFORMAT_ID_GRAY8      = 0,
    SUBFORMAT_ID_GRAY16     = 1,
    SUBFORMAT_ID_BGGR       = 0, // !!! Note: Make sure bayer sub-formats don't  !!!
    SUBFORMAT_ID_GBRG       = 1, // !!! overflow the sensor.hw_flags.bayer field !!!
    SUBFORMAT_ID_GRBG       = 2,
    SUBFORMAT_ID_RGGB       = 3,
    SUBFORMAT_ID_YUV422     = 0,
    SUBFORMAT_ID_YVU422     = 1,
    /* Note: Update PIXFORMAT_IS_VALID when adding new formats */
    SUBFORMAT_ID_HWC        = 0,    // for rtak
    SUBFORMAT_ID_CHW        = 1,    // for rtak
} subformat_id_t;

// Pixel format Byte Per Pixel.
typedef enum {
    PIXFORMAT_BPP_BINARY    = 0,
    PIXFORMAT_BPP_GRAY8     = 1,
    PIXFORMAT_BPP_GRAY16    = 2,
    PIXFORMAT_BPP_RGB565    = 2,
    PIXFORMAT_BPP_BAYER     = 1,
    PIXFORMAT_BPP_YUV422    = 2,
    /* Note: Update PIXFORMAT_IS_VALID when adding new formats */
    PIXFORMAT_BPP_RGB888    = 3,    // for rtak
} pixformat_bpp_t;

// Pixel format flags.
#define PIXFORMAT_FLAGS_Y       (1 << 28) // YUV format.
#define PIXFORMAT_FLAGS_M       (1 << 27) // Mutable format.
#define PIXFORMAT_FLAGS_C       (1 << 26) // Colored format.
#define PIXFORMAT_FLAGS_J       (1 << 25) // Compressed format (JPEG/PNG).
#define PIXFORMAT_FLAGS_R       (1 << 24) // RAW/Bayer format.
#define PIXFORMAT_FLAGS_CY      (PIXFORMAT_FLAGS_C | PIXFORMAT_FLAGS_Y)
#define PIXFORMAT_FLAGS_CM      (PIXFORMAT_FLAGS_C | PIXFORMAT_FLAGS_M)
#define PIXFORMAT_FLAGS_CR      (PIXFORMAT_FLAGS_C | PIXFORMAT_FLAGS_R)
#define PIXFORMAT_FLAGS_CJ      (PIXFORMAT_FLAGS_C | PIXFORMAT_FLAGS_J)
#define IMLIB_IMAGE_MAX_SIZE(x) ((x) & 0xFFFFFFFF)

// Each pixel format encodes flags, pixel format id and bpp as follows:
// 31......29  28  27  26  25  24  23..........16  15...........8  7.............0
// <RESERVED>  YF  MF  CF  JF  RF  <PIXFORMAT_ID>  <SUBFORMAT_ID>  <BYTES_PER_PIX>
// NOTE: Bit 31-30 must Not be used for pixformat_t to be used as mp_int_t.
typedef enum {
    PIXFORMAT_INVALID    = (0),
    PIXFORMAT_BINARY     = (PIXFORMAT_FLAGS_M  | (PIXFORMAT_ID_BINARY << 16) | (0                   << 8) | PIXFORMAT_BPP_BINARY ),
    PIXFORMAT_GRAYSCALE  = (PIXFORMAT_FLAGS_M  | (PIXFORMAT_ID_GRAY   << 16) | (SUBFORMAT_ID_GRAY8  << 8) | PIXFORMAT_BPP_GRAY8  ),
    PIXFORMAT_RGB565     = (PIXFORMAT_FLAGS_CM | (PIXFORMAT_ID_RGB565 << 16) | (0                   << 8) | PIXFORMAT_BPP_RGB565 ),
    PIXFORMAT_BAYER      = (PIXFORMAT_FLAGS_CR | (PIXFORMAT_ID_BAYER  << 16) | (SUBFORMAT_ID_BGGR   << 8) | PIXFORMAT_BPP_BAYER  ),
    PIXFORMAT_BAYER_BGGR = (PIXFORMAT_FLAGS_CR | (PIXFORMAT_ID_BAYER  << 16) | (SUBFORMAT_ID_BGGR   << 8) | PIXFORMAT_BPP_BAYER  ),
    PIXFORMAT_BAYER_GBRG = (PIXFORMAT_FLAGS_CR | (PIXFORMAT_ID_BAYER  << 16) | (SUBFORMAT_ID_GBRG   << 8) | PIXFORMAT_BPP_BAYER  ),
    PIXFORMAT_BAYER_GRBG = (PIXFORMAT_FLAGS_CR | (PIXFORMAT_ID_BAYER  << 16) | (SUBFORMAT_ID_GRBG   << 8) | PIXFORMAT_BPP_BAYER  ),
    PIXFORMAT_BAYER_RGGB = (PIXFORMAT_FLAGS_CR | (PIXFORMAT_ID_BAYER  << 16) | (SUBFORMAT_ID_RGGB   << 8) | PIXFORMAT_BPP_BAYER  ),
    PIXFORMAT_YUV422     = (PIXFORMAT_FLAGS_CY | (PIXFORMAT_ID_YUV422 << 16) | (SUBFORMAT_ID_YUV422 << 8) | PIXFORMAT_BPP_YUV422 ),
    PIXFORMAT_YVU422     = (PIXFORMAT_FLAGS_CY | (PIXFORMAT_ID_YUV422 << 16) | (SUBFORMAT_ID_YVU422 << 8) | PIXFORMAT_BPP_YUV422 ),
    PIXFORMAT_JPEG       = (PIXFORMAT_FLAGS_CJ | (PIXFORMAT_ID_JPEG   << 16) | (0                   << 8) | 0                    ),
    // for rtak
    PIXFORMAT_RGB888_HWC = (PIXFORMAT_FLAGS_M  | (PIXFORMAT_ID_RGB888 << 16) | (SUBFORMAT_ID_HWC    << 8) | PIXFORMAT_BPP_RGB888 ),
    PIXFORMAT_RGB888_CHW = (PIXFORMAT_FLAGS_M  | (PIXFORMAT_ID_RGB888 << 16) | (SUBFORMAT_ID_CHW    << 8) | PIXFORMAT_BPP_RGB888 ),
    PIXFORMAT_LAST       = 0xFFFFFFFFU,
} pixformat_t;

#define PIXFORMAT_MUTABLE_ANY           \
        PIXFORMAT_BINARY:               \
        case PIXFORMAT_GRAYSCALE:       \
        case PIXFORMAT_RGB565           \

#define PIXFORMAT_BAYER_ANY             \
        PIXFORMAT_BAYER_BGGR:           \
        case PIXFORMAT_BAYER_GBRG:      \
        case PIXFORMAT_BAYER_GRBG:      \
        case PIXFORMAT_BAYER_RGGB       \

#define PIXFORMAT_YUV_ANY               \
        PIXFORMAT_YUV422:               \
        case PIXFORMAT_YVU422           \

#define IMLIB_PIXFORMAT_IS_VALID(x)     \
    ((x == PIXFORMAT_BINARY)            \
     || (x == PIXFORMAT_GRAYSCALE)      \
     || (x == PIXFORMAT_RGB565)         \
     || (x == PIXFORMAT_BAYER_BGGR)     \
     || (x == PIXFORMAT_BAYER_GBRG)     \
     || (x == PIXFORMAT_BAYER_GRBG)     \
     || (x == PIXFORMAT_BAYER_RGGB)     \
     || (x == PIXFORMAT_YUV422)         \
     || (x == PIXFORMAT_YVU422)         \
     || (x == PIXFORMAT_JPEG))          \

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define PIXFORMAT_STRUCT            \
struct {                            \
  union {                           \
    struct {                        \
        uint32_t bpp            :8; \
        uint32_t subfmt_id      :8; \
        uint32_t pixfmt_id      :8; \
        uint32_t is_bayer       :1; \
        uint32_t is_compressed  :1; \
        uint32_t is_color       :1; \
        uint32_t is_mutable     :1; \
        uint32_t is_yuv         :1; \
        uint32_t /*reserved*/   :3; \
    };                              \
    uint32_t pixfmt;                \
  };                                \
  uint32_t size; /* for compressed images */ \
}
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define PIXFORMAT_STRUCT            \
struct {                            \
  union {                           \
    struct {                        \
        uint32_t /*reserved*/   :3; \
        uint32_t is_yuv         :1; \
        uint32_t is_mutable     :1; \
        uint32_t is_color       :1; \
        uint32_t is_compressed  :1; \
        uint32_t is_bayer       :1; \
        uint32_t pixfmt_id      :8; \
        uint32_t subfmt_id      :8; \
        uint32_t bpp            :8; \
    };                              \
    uint32_t pixfmt;                \
  };                                \
  uint32_t size; /* for compressed images */ \
}
#else
#error "Byte order is not defined."
#endif

typedef struct image {
    uint32_t w;
    uint32_t h;
    PIXFORMAT_STRUCT;
    union {
        uint8_t *pixels;
        uint8_t *data;
    };
} image_t;

void image_init(image_t *ptr, int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels);
void image_copy(image_t *dst, image_t *src);
size_t image_size(image_t *ptr);
// bool image_get_mask_pixel(image_t *ptr, int x, int y);

#define IMAGE_BINARY_LINE_LEN(image) (((image)->w + UINT32_T_MASK) >> UINT32_T_SHIFT)
#define IMAGE_BINARY_LINE_LEN_BYTES(image) (IMAGE_BINARY_LINE_LEN(image) * sizeof(uint32_t))

#define IMAGE_GRAYSCALE_LINE_LEN(image) ((image)->w)
#define IMAGE_GRAYSCALE_LINE_LEN_BYTES(image) (IMAGE_GRAYSCALE_LINE_LEN(image) * sizeof(uint8_t))

#define IMAGE_RGB565_LINE_LEN(image) ((image)->w)
#define IMAGE_RGB565_LINE_LEN_BYTES(image) (IMAGE_RGB565_LINE_LEN(image) * sizeof(uint16_t))

#define IMAGE_GET_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    (((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] >> (_x & UINT32_T_MASK)) & 1; \
})

#define IMAGE_PUT_BINARY_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    size_t _i = (((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT); \
    size_t _j = _x & UINT32_T_MASK; \
    ((uint32_t *) _image->data)[_i] = (((uint32_t *) _image->data)[_i] & (~(1 << _j))) | ((_v & 1) << _j); \
})

#define IMAGE_CLEAR_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] &= ~(1 << (_x & UINT32_T_MASK)); \
})

#define IMAGE_SET_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] |= 1 << (_x & UINT32_T_MASK); \
})

#define IMAGE_GET_GRAYSCALE_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_GRAYSCALE_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#define IMAGE_GET_RGB565_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_RGB565_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#define IMAGE_GET_YUV_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_YUV_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#define IMAGE_GET_BAYER_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_BAYER_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

// Fast Stuff //

#define IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data) + (((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y); \
})

#define IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    (_row_ptr[_x >> UINT32_T_SHIFT] >> (_x & UINT32_T_MASK)) & 1; \
})

#define IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    size_t _i = _x >> UINT32_T_SHIFT; \
    size_t _j = _x & UINT32_T_MASK; \
    _row_ptr[_i] = (_row_ptr[_i] & (~(1 << _j))) | ((_v & 1) << _j); \
})

#define IMAGE_CLEAR_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x >> UINT32_T_SHIFT] &= ~(1 << (_x & UINT32_T_MASK)); \
})

#define IMAGE_SET_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x >> UINT32_T_SHIFT] |= 1 << (_x & UINT32_T_MASK); \
})

#define IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x]; \
})

#define IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    _row_ptr[_x] = _v; \
})

#define IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x]; \
})

#define IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    _row_ptr[_x] = _v; \
})

#define IMAGE_COMPUTE_BAYER_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_COMPUTE_YUV_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data) + (_image->w * _y); \
})
/* end openmv @file:imlib.h @line:698 */
/* start openmv @file:imlib.h @line:709 */
// Grayscale maxes
#define IM_MAX_GS (255)

#define IM_IS_BINARY(img)   ((img)->pixfmt == PIXFORMAT_BINARY)
#define IM_IS_GS(img)       ((img)->pixfmt == PIXFORMAT_GRAYSCALE)
#define IM_IS_RGB565(img)   ((img)->pixfmt == PIXFORMAT_RGB565)
#define IM_IS_BAYER(img)    ((img)->is_bayer)
#define IM_IS_JPEG(img)     ((img)->pixfmt == PIXFORMAT_JPEG)

#define IM_X_INSIDE(img, x) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       (0<=_x)&&(_x<_img->w); })

#define IM_Y_INSIDE(img, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (y) _y = (y); \
       (0<=_y)&&(_y<_img->h); })

#define IM_GET_GS_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RGB565_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint16_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_SET_GS_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]=_p; })

#define IM_SET_RGB565_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((uint16_t*)_img->pixels)[(_y*_img->w)+_x]=_p; })

#define IM_EQUAL(img0, img1) \
    ({ __typeof__ (img0) _img0 = (img0); \
       __typeof__ (img1) _img1 = (img1); \
       (_img0->w==_img1->w)&&(_img0->h==_img1->h)&&(_img0->pixfmt=_img1->pixfmt); })

#define IM_TO_GS_PIXEL(img, x, y)    \
    (img->bpp == 1 ? img->pixels[((y)*img->w)+(x)] : COLOR_RGB565_TO_Y(((uint16_t*)img->pixels)[((y)*img->w)+(x)]) )

/* end openmv @file:imlib.h @line:960 */
/* start openmv @file:imlib.h @line:1237 */
void imlib_set_pixel(image_t *img, int x, int y, int p);
void imlib_draw_line(image_t *img, int x0, int y0, int x1, int y1, int c, int thickness);
void imlib_draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c, int thickness, bool fill);
void imlib_draw_circle(image_t *img, int cx, int cy, int r, int c, int thickness, bool fill);
void imlib_deyuv_line(int x_start, int x_end, int y_row, void *dst_row_ptr, pixformat_t pixfmt, image_t *src);

/**
 * @brief draw string
 * 
 * @param img image_t
 * @param x_off x
 * @param y_off y
 * @param str draw string
 * @param c color
 * @param scale string scale, eg: 1.0, 2.0....
 * @param x_spacing 允许你在字符之间添加（如果是正数）或减去（如果是负数）x像素，设置字符间距。
 * @param y_spacing 允许你在字符之间添加（如果是正数）或减去（如果是负数）y像素，设置行间距。
 * @param mono_space 认为True，强制文本间距固定。对于大文本，这看起来很糟糕。设置False以获得非固定宽度的字符间距，看起来好多了。
 * @param char_rotation 
 * @param char_hmirror 如果为True，则水平镜像字符串中的所有字符。
 * @param char_vflip 如果为True，则垂直翻转字符串中的所有字符。
 * @param string_rotation 可以是0、90、180、270，来旋转字符串。
 * @param string_hmirror 如果为True，则水平镜像字符串。
 * @param string_hflip 如果为True，则垂直翻转字符串。
 */
void imlib_draw_string(image_t *img, int x_off, int y_off, const char *str, int c, float scale, int x_spacing, int y_spacing, bool mono_space,
                       int char_rotation, bool char_hmirror, bool char_vflip, int string_rotation, bool string_hmirror, bool string_hflip);

/**
 * @brief 图像旋转校正
 * 
 * @param img image_t
 * @param x_rotation 是围绕x轴在帧缓冲器中旋转图像的度数（这使图像上下旋转）。
 * @param y_rotation 是帧缓冲区中围绕y轴旋转图像的度数（即左右旋转图像）。
 * @param z_rotation 是围绕z轴在帧缓冲器中旋转图像的度数（即，使图像旋转到适当位置）。
 * @param x_translation 是旋转后将图像移动到左侧或右侧的单位数。因为这个变换是应用在三维空间的，单位不是像素…
 * @param y_translation 是旋转后将图像上移或下移的单位数。因为这个变换是应用在三维空间的，单位不是像素…
 * @param zoom 是通过图像缩放的量。默认情况下1.0。
 * @param fov 是在 3D 空间中旋转图像之前进行 2D->3D 投影时在内部使用的视场。 当这个值接近 0 时，图像被放置在远离视口的无穷远处。 当这个值接近 180 时，图像被放置在视口内。 通常，您不应更改此值，但可以修改它以更改 2D->3D 映射效果。
 * @param corners 是四个 (x,y) 元组的列表，表示用于创建 4 点的四个角对应单应性， 将第一个角映射到 (0, 0)，第二个角映射到 (image_width-1, 0)， 第三个角映射到 (image_width-1, image_height-1)，第四个角映射到 (0, image_height -1). 然后在重新映射图像后应用 3D 旋转。 此参数允许您使用 rotation_corr 来执行诸如鸟瞰图变换之类的操作。
 */
void imlib_rotation_corr(image_t *img, float x_rotation, float y_rotation, float z_rotation,
                         float x_translation, float y_translation,
                         float zoom, float fov, float *corners);


#endif //__IMLIB_H__
