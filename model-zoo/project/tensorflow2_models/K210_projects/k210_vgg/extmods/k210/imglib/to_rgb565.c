#if 0
#include <rtthread.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "imlib.h"

void to_rgb565(image_t *ori, image_t *res){
    switch (ori->pixfmt)
    {
    case PIXFORMAT_GRAYSCALE:
        for(uint32_t ih = 0; ih < ori->h; ih++){
            for(uint32_t iw = 0; iw < ori->w; iw++){
                uint8_t pix = IMAGE_GET_GRAYSCALE_PIXEL(ori, iw, ih);
                IMAGE_PUT_RGB565_PIXEL(res, iw, ih, COLOR_GRAYSCALE_TO_RGB565(pix));
            }
        }
        break;
    
    case PIXFORMAT_RGB565:
        ;
        break;
    
    case PIXFORMAT_RGB888_CHW:
        for(uint32_t ih = 0; ih < ori->h; ih++){
            for(uint32_t iw = 0; iw < ori->w; iw++){
                uint8_t r = IMAGE_GET_RGB888_CHW_PIXEL(ori, iw, ih, 0);
                uint8_t g = IMAGE_GET_RGB888_CHW_PIXEL(ori, iw, ih, 1);
                uint8_t b = IMAGE_GET_RGB888_CHW_PIXEL(ori, iw, ih, 2);
                IMAGE_PUT_RGB565_PIXEL(res, iw, ih, COLOR_R8_G8_B8_TO_RGB565(r, g, b));
            }
        }
        break;
    
    case PIXFORMAT_RGB888_HWC:
        for(uint32_t ih = 0; ih < ori->h; ih++){
            for(uint32_t iw = 0; iw < ori->w; iw++){
                uint8_t r = IMAGE_GET_RGB888_HWC_PIXEL(ori, iw, ih, 0);
                uint8_t g = IMAGE_GET_RGB888_HWC_PIXEL(ori, iw, ih, 1);
                uint8_t b = IMAGE_GET_RGB888_HWC_PIXEL(ori, iw, ih, 2);
                IMAGE_PUT_RGB565_PIXEL(res, ih, iw, COLOR_R8_G8_B8_TO_RGB565(r, g, b));
            }
        }
        break;
    default:
        mp_raise_TypeError("can't gray the format current");
        break;
    }
}
#endif // #if 0
