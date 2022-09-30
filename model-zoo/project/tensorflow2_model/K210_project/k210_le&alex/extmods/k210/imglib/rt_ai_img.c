
#include <rt_ai_img.h>
#include <rtthread.h>
#include <rt_ai_img_utils.h>


static inline int is_in_array(int x, int y, int height, int width)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
        return 1;
    else
        return 0;
}

void bilinera_interpolation_grayscale(image_t *ori, image_t *res)
{
    uint32_t height = ori->h;
    uint32_t width = ori->w;
    uint32_t out_height = res->h;
    uint32_t out_width = res->w;
    // rescale factor
    double h_times = (double)out_height / (double)height,
           w_times = (double)out_width / (double)width;
    int  x1, y1, x2, y2;
    uint32_t f11, f12, f21, f22;
    double x, y;

    for (int i = 0; i < out_height; i++){
        for (int j = 0; j < out_width; j++){
            x = j / w_times;    // j:origin location at width axis. and x is new.
            y = i / h_times;    // i:origin location at height axis. and y is new.
          
            x1 = (int)(x - 1);  // left x
            x2 = (int)(x + 1);  // right x
            y1 = (int)(y + 1);  // bottom y
            y2 = (int)(y - 1);  // top y
            /* 
                f11 f12
                f21 f22
             */
            //IMAGE_GET_GRAYSCALE_PIXEL(image, x, y)
            f11 = is_in_array(x1, y1, height, width) ? IMAGE_GET_GRAYSCALE_PIXEL(ori, x1, y1) : 0;
            f12 = is_in_array(x1, y2, height, width) ? IMAGE_GET_GRAYSCALE_PIXEL(ori, x1, y2) : 0;
            f21 = is_in_array(x2, y1, height, width) ? IMAGE_GET_GRAYSCALE_PIXEL(ori, x2, y1) : 0;
            f22 = is_in_array(x2, y2, height, width) ? IMAGE_GET_GRAYSCALE_PIXEL(ori, x2, y2) : 0;
            uint8_t e = (uint8_t)(((f11 * (x2 - x) * (y2 - y)) +
                            (f21 * (x - x1) * (y2 - y)) +
                            (f12 * (x2 - x) * (y - y1)) +
                            (f22 * (x - x1) * (y - y1))) / ((x2 - x1) * (y2 - y1)));
            IMAGE_PUT_GRAYSCALE_PIXEL(res, j, i, e);
        }
    }
}

void bilinera_interpolation_rgb565(image_t *ori, image_t *res)
{
    uint32_t height = ori->h;
    uint32_t width = ori->w;
    uint32_t out_height = res->h;
    uint32_t out_width = res->w;
    // rescale factor
    double h_times = (double)out_height / (double)height,
           w_times = (double)out_width / (double)width;
    int  x1, y1, x2, y2;
    uint32_t f11, f12, f21, f22, e11, e12, e21, e22;
    double x, y;

    for (int i = 0; i < out_height; i++){
        for (int j = 0; j < out_width; j++){
            x = j / w_times;    // j:origin location at width axis. and x is new.
            y = i / h_times;    // i:origin location at height axis. and y is new.
          
            x1 = (int)(x - 1);  // left x
            x2 = (int)(x + 1);  // right x
            y1 = (int)(y + 1);  // bottom y
            y2 = (int)(y - 1);  // top y
            /* 
                f11 f12
                f21 f22
             */
            f11 = is_in_array(x1, y1, height, width) ? IMAGE_GET_RGB565_PIXEL(ori, x1, y1) : 0;
            f12 = is_in_array(x1, y2, height, width) ? IMAGE_GET_RGB565_PIXEL(ori, x1, y2) : 0;
            f21 = is_in_array(x2, y1, height, width) ? IMAGE_GET_RGB565_PIXEL(ori, x2, y1) : 0;
            f22 = is_in_array(x2, y2, height, width) ? IMAGE_GET_RGB565_PIXEL(ori, x2, y2) : 0;

            e11 = COLOR_RGB565_TO_R8(f11);
            e12 = COLOR_RGB565_TO_R8(f12);
            e21 = COLOR_RGB565_TO_R8(f21);
            e22 = COLOR_RGB565_TO_R8(f22);
            uint8_t r = (uint8_t)(((e11 * (x2 - x) * (y2 - y)) +
                                   (e21 * (x - x1) * (y2 - y)) +
                                   (e12 * (x2 - x) * (y - y1)) +
                                   (e22 * (x - x1) * (y - y1))) / ((x2 - x1) * (y2 - y1)));
            
            e11 = COLOR_RGB565_TO_G8(f11);
            e12 = COLOR_RGB565_TO_G8(f12);
            e21 = COLOR_RGB565_TO_G8(f21);
            e22 = COLOR_RGB565_TO_G8(f22);
            uint8_t g = (uint8_t)(((e11 * (x2 - x) * (y2 - y)) +
                                   (e21 * (x - x1) * (y2 - y)) +
                                   (e12 * (x2 - x) * (y - y1)) +
                                   (e22 * (x - x1) * (y - y1))) / ((x2 - x1) * (y2 - y1)));

            e11 = COLOR_RGB565_TO_B8(f11);
            e12 = COLOR_RGB565_TO_B8(f12);
            e21 = COLOR_RGB565_TO_B8(f21);
            e22 = COLOR_RGB565_TO_B8(f22);
            uint8_t b = (uint8_t)(((e11 * (x2 - x) * (y2 - y)) +
                                   (e21 * (x - x1) * (y2 - y)) +
                                   (e12 * (x2 - x) * (y - y1)) +
                                   (e22 * (x - x1) * (y - y1))) / ((x2 - x1) * (y2 - y1)));
            
            IMAGE_PUT_RGB565_PIXEL(res, j, i, COLOR_R8_G8_B8_TO_RGB565(r, g, b));
        }
    }
}

void bilinera_interpolation_rgb888_chw(image_t *ori, image_t *res)
{
    uint32_t height = ori->h;
    uint32_t width = ori->w;
    uint32_t out_height = res->h;
    uint32_t out_width = res->w;

    uint8_t* in_array = ori->data;
    uint8_t* out_array = res->data;
    // rescale factor
    double h_times = (double)out_height / (double)height,
           w_times = (double)out_width / (double)width;
    int  x1, y1, x2, y2;
    uint32_t f11, f12, f21, f22;
    double x, y;
    for(int rgb = 0; rgb < 3; rgb++){
        in_array = ori->data + (height * width * rgb);
        out_array = res->data + (out_height * out_width * rgb);
        for (int i = 0; i < out_height; i++){
            for (int j = 0; j < out_width; j++){
                x = j / w_times;    // j:origin location at width axis. and x is new.
                y = i / h_times;    // i:origin location at height axis. and y is new.
            
                x1 = (int)(x - 1);  // left x
                x2 = (int)(x + 1);  // right x
                y1 = (int)(y + 1);  // bottom y
                y2 = (int)(y - 1);  // top y
                f11 = is_in_array(x1, y1, height, width) ? in_array[y1*width+x1] : 0;
                f12 = is_in_array(x1, y2, height, width) ? in_array[y2*width+x1] : 0;
                f21 = is_in_array(x2, y1, height, width) ? in_array[y1*width+x2] : 0;
                f22 = is_in_array(x2, y2, height, width) ? in_array[y2*width+x2] : 0;
                out_array[i*out_width+j] = (uint8_t)(((f11 * (x2 - x) * (y2 - y)) +
                                        (f21 * (x - x1) * (y2 - y)) +
                                        (f12 * (x2 - x) * (y - y1)) +
                                        (f22 * (x - x1) * (y - y1))) / ((x2 - x1) * (y2 - y1)));
            }
        }
    }
}

void bilinera_interpolation_rgb888_hwc(image_t *ori, image_t *res)
{
    uint32_t height = ori->h;
    uint32_t width = ori->w;
    uint32_t out_height = res->h;
    uint32_t out_width = res->w;

    uint8_t* in_array = ori->data;
    uint8_t* out_array = res->data;
    // rescale factor
    double h_times = (double)out_height / (double)height,
           w_times = (double)out_width / (double)width;
    int  x1, y1, x2, y2;
    uint32_t f11, f12, f21, f22, e11, e12, e21, e22;
    double x, y;

    for (int i = 0; i < out_height; i++){
        for (int j = 0; j < out_width; j++){
            x = j / w_times;    // j:origin location at width axis. and x is new.
            y = i / h_times;    // i:origin location at height axis. and y is new.
        
            x1 = (int)(x - 1);  // left x
            x2 = (int)(x + 1);  // right x
            y1 = (int)(y + 1);  // bottom y
            y2 = (int)(y - 1);  // top y
            // R
            f11 = is_in_array(x1, y1, height, width) ? in_array[3*(y1*width+x1)+0] : 0;
            f12 = is_in_array(x1, y2, height, width) ? in_array[3*(y2*width+x1)+0] : 0;
            f21 = is_in_array(x2, y1, height, width) ? in_array[3*(y1*width+x2)+0] : 0;
            f22 = is_in_array(x2, y2, height, width) ? in_array[3*(y2*width+x2)+0] : 0;
            out_array[3*i*out_width+j+0] = (uint8_t)(((f11 * (x2 - x) * (y2 - y)) +
                                    (f21 * (x - x1) * (y2 - y)) +
                                    (f12 * (x2 - x) * (y - y1)) +
                                    (f22 * (x - x1) * (y - y1))) / ((x2 - x1) * (y2 - y1)));
            // G
            f11 = is_in_array(x1, y1, height, width) ? in_array[3*(y1*width+x1)+1] : 0;
            f12 = is_in_array(x1, y2, height, width) ? in_array[3*(y2*width+x1)+1] : 0;
            f21 = is_in_array(x2, y1, height, width) ? in_array[3*(y1*width+x2)+1] : 0;
            f22 = is_in_array(x2, y2, height, width) ? in_array[3*(y2*width+x2)+1] : 0;
            out_array[3*(i*out_width+j)+1] = (uint8_t)(((f11 * (x2 - x) * (y2 - y)) +
                                    (f21 * (x - x1) * (y2 - y)) +
                                    (f12 * (x2 - x) * (y - y1)) +
                                    (f22 * (x - x1) * (y - y1))) / ((x2 - x1) * (y2 - y1)));
            // B
            f11 = is_in_array(x1, y1, height, width) ? in_array[3*(y1*width+x1)+2] : 0;
            f12 = is_in_array(x1, y2, height, width) ? in_array[3*(y2*width+x1)+2] : 0;
            f21 = is_in_array(x2, y1, height, width) ? in_array[3*(y1*width+x2)+2] : 0;
            f22 = is_in_array(x2, y2, height, width) ? in_array[3*(y2*width+x2)+2] : 0;
            out_array[3*(i*out_width+j)+2] = (uint8_t)(((f11 * (x2 - x) * (y2 - y)) +
                                    (f21 * (x - x1) * (y2 - y)) +
                                    (f12 * (x2 - x) * (y - y1)) +
                                    (f22 * (x - x1) * (y - y1))) / ((x2 - x1) * (y2 - y1)));
        }
    }
}


void resize_bilinera_interpolation(image_t *ori, image_t *res){
    
    switch (ori->pixfmt)
    {
    case PIXFORMAT_GRAYSCALE:
        bilinera_interpolation_grayscale(ori, res);
        break;
    
    case PIXFORMAT_RGB565:
        bilinera_interpolation_rgb565(ori, res);
        break;
    
    case PIXFORMAT_RGB888_CHW:
        bilinera_interpolation_rgb888_chw(ori, res);
        break;
    
    case PIXFORMAT_RGB888_HWC:
        bilinera_interpolation_rgb888_hwc(ori, res);
        break;
    default:
        IMG_LOG_ERR("can't resize the format current");
        break;
    }
}


void crop(image_t *ori, image_t *res, uint32_t offset_height, uint32_t offset_width){

    IMG_ASSERT_RET((offset_height + res->h < ori->h), );
    IMG_ASSERT_RET((offset_width + res->w < ori->w), );
    switch (ori->pixfmt)
    {
    case PIXFORMAT_GRAYSCALE:
        for(uint32_t ih = 0; ih < res->h; ih++){
            for(uint32_t iw = 0; iw < res->w; iw++){
                IMAGE_PUT_GRAYSCALE_PIXEL(res, iw, ih, IMAGE_GET_GRAYSCALE_PIXEL(ori, (iw+offset_width), (ih+offset_height)) );
            }
        }
        break;
    
    case PIXFORMAT_RGB565:
        for(uint32_t ih = 0; ih < res->h; ih++){
            for(uint32_t iw = 0; iw < res->w; iw++){
                IMAGE_PUT_RGB565_PIXEL(res, iw, ih, IMAGE_GET_RGB565_PIXEL(ori, (iw+offset_width), (ih+offset_height)) );
            }
        }
        break;
    
    case PIXFORMAT_RGB888_CHW:
        for(uint32_t ih = 0; ih < res->h; ih++){
            for(uint32_t iw = 0; iw < res->w; iw++){
                IMAGE_PUT_RGB888_CHW_PIXEL(res, iw, ih, 0, IMAGE_GET_RGB888_CHW_PIXEL(ori, (iw+offset_width), (ih+offset_height), 0));  // R
                IMAGE_PUT_RGB888_CHW_PIXEL(res, iw, ih, 1, IMAGE_GET_RGB888_CHW_PIXEL(ori, (iw+offset_width), (ih+offset_height), 1));  // G
                IMAGE_PUT_RGB888_CHW_PIXEL(res, iw, ih, 2, IMAGE_GET_RGB888_CHW_PIXEL(ori, (iw+offset_width), (ih+offset_height), 2));  // B
            }
        }
        break;
    
    case PIXFORMAT_RGB888_HWC:
        for(uint32_t ih = 0; ih < res->h; ih++){
            for(uint32_t iw = 0; iw < res->w; iw++){
                IMAGE_PUT_RGB888_HWC_PIXEL(res, iw, ih, 0, IMAGE_GET_RGB888_HWC_PIXEL(ori, (iw+offset_width), (ih+offset_height), 0));  // R
                IMAGE_PUT_RGB888_HWC_PIXEL(res, iw, ih, 1, IMAGE_GET_RGB888_HWC_PIXEL(ori, (iw+offset_width), (ih+offset_height), 1));  // G
                IMAGE_PUT_RGB888_HWC_PIXEL(res, iw, ih, 2, IMAGE_GET_RGB888_HWC_PIXEL(ori, (iw+offset_width), (ih+offset_height), 2));  // B
            }
        }
        break;
    default:
        IMG_LOG_ERR("can't crop the format current");
        break;
    }
}

void to_gray(image_t *ori, image_t *res){
    switch (ori->pixfmt)
    {
    case PIXFORMAT_GRAYSCALE:
        ;
        break;
    
    case PIXFORMAT_RGB565:
        for(uint32_t ih = 0; ih < ori->h; ih++){
            for(uint32_t iw = 0; iw < ori->w; iw++){
                uint16_t pix = IMAGE_GET_RGB565_PIXEL(ori, iw, ih);
                IMAGE_PUT_GRAYSCALE_PIXEL(res, iw, ih, COLOR_RGB565_TO_GRAYSCALE(pix));
            }
        }
        break;
    
    case PIXFORMAT_RGB888_CHW:
        for(uint32_t ih = 0; ih < ori->h; ih++){
            for(uint32_t iw = 0; iw < ori->w; iw++){
                uint8_t r = IMAGE_GET_RGB888_CHW_PIXEL(ori, iw, ih, 0);
                uint8_t g = IMAGE_GET_RGB888_CHW_PIXEL(ori, iw, ih, 1);
                uint8_t b = IMAGE_GET_RGB888_CHW_PIXEL(ori, iw, ih, 2);
                IMAGE_PUT_GRAYSCALE_PIXEL(res, iw, ih, COLOR_RGB888_TO_Y(r, g, b));
            }
        }
        break;
    
    case PIXFORMAT_RGB888_HWC:
        for(uint32_t ih = 0; ih < ori->h; ih++){
            for(uint32_t iw = 0; iw < ori->w; iw++){
                uint8_t r = IMAGE_GET_RGB888_HWC_PIXEL(ori, iw, ih, 0);
                uint8_t g = IMAGE_GET_RGB888_HWC_PIXEL(ori, iw, ih, 1);
                uint8_t b = IMAGE_GET_RGB888_HWC_PIXEL(ori, iw, ih, 2);
                IMAGE_PUT_GRAYSCALE_PIXEL(res, ih, iw, COLOR_RGB888_TO_Y(r, g, b));
            }
        }
        break;
    default:
        IMG_LOG_ERR("can't gray the format current");
        break;
    }
}

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
        IMG_LOG_ERR("can't gray the format current");
        break;
    }
}
 
void set_pixel(image_t *img, int x, int y, int p)
{
    imlib_set_pixel(img, x,  y,  p);
}


void draw_line(image_t *img, int x0, int y0, int x1, int y1, int c, int thickness)
{
    imlib_draw_line(img, x0,  y0,  x1,  y1,  c,  thickness);
}


void draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c, int thickness, bool fill)
{
    imlib_draw_rectangle(img, rx,  ry,  rw,  rh,  c,  thickness,  fill);
}


void draw_circle(image_t *img, int cx, int cy, int r, int c, int thickness, bool fill)
{
    imlib_draw_circle(img, cx,  cy,  r,  c,  thickness,  fill);
}

void draw_string(image_t *img, int x_off, int y_off, const char *str, int c, float scale, int x_spacing, int y_spacing, bool mono_space,
                       int char_rotation, bool char_hmirror, bool char_vflip, int string_rotation, bool string_hmirror, bool string_hflip)
{
    imlib_draw_string(img,  x_off,  y_off, str,  c,  scale,  x_spacing,  y_spacing,  mono_space,
                        char_rotation,  char_hmirror,  char_vflip,  string_rotation,  string_hmirror,  string_hflip);
}


void rotation_corr(image_t *img, float x_rotation, float y_rotation, float z_rotation,
                         float x_translation, float y_translation,
                         float zoom, float fov, float *corners)
{
    imlib_rotation_corr(img,  x_rotation,  y_rotation,  z_rotation,
                          x_translation,  y_translation,
                          zoom,  fov,  corners);
}
                         