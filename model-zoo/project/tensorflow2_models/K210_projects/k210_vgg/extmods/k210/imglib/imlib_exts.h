#ifndef __IMLIB_EXTS_H
#define __IMLIB_EXTS_H
#include <stdbool.h>
// for rt-ak
#define IMAGE_RGB888_LINE_LEN(image) ((image)->w)
#define IMAGE_RGB888_LINE_LEN_BYTES(image) (IMAGE_RGB888_LINE_LEN(image) * sizeof(uint8_t) * PIXFORMAT_BPP_RGB888)


#define IMAGE_GET_RGB888_CHW_PIXEL(image, x, y, i) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (i) _i = (i); \
    ((uint8_t *) _image->data)[(_i * _image->w * _image->h) + (_image->w * _y) + _x]; \
})

#define IMAGE_PUT_RGB888_CHW_PIXEL(image, x, y, i, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    __typeof__ (i) _i = (i); \
    ((uint8_t *) _image->data)[(_i * _image->w * _image->h) + (_image->w * _y) + _x] = _v; \
})

#define IMAGE_GET_RGB888_HWC_PIXEL(image, x, y, i) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (i) _i = (i); \
    ((uint8_t *) _image->data)[3 * (_image->w * _y + _x) + _i]; \
})

#define IMAGE_PUT_RGB888_HWC_PIXEL(image, x, y, i, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    __typeof__ (i) _i = (i); \
    ((uint8_t *) _image->data)[3 * (_image->w * _y + _x) + _i] = _v; \
})

#endif //__IMLIB_EXTS_H
