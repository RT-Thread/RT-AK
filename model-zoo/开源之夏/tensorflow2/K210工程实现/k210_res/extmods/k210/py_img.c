#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objarray.h"
#include "py/binary.h"
#include <rtthread.h>

#if (defined(EXTMODS_K210_DVP))
#include "rt_ai_img.h"
#include <drv_lcd.h>
#include "imglib/imlib.h"
#include "py_img.h"
#include "py_assert.h"
typedef struct
{
    mp_obj_base_t base;
    image_t _cobj;
} py_img_obj_t;

const mp_obj_type_t py_img_type;

mp_obj_t py_img(int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels)
{
    py_img_obj_t *o = m_new_obj(py_img_obj_t);
    o->base.type = &py_img_type;
    o->_cobj.w = w;
    o->_cobj.h = h;
    o->_cobj.size = size;
    o->_cobj.pixfmt = pixfmt;
    o->_cobj.pixels = pixels;
    return o;
}

void *py_img_cobj(mp_obj_t img_obj)
{
    PY_ASSERT_TYPE(img_obj, &py_img_type);
    return &((py_img_obj_t *)img_obj)->_cobj;
}


static mp_obj_t py_image_show(mp_obj_t self)
{
    image_t *p = py_img_cobj(self);

    if (p->pixels)
    {
        lcd_show_image(0, 0, p->h, p->w, p->pixels);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_show_obj, py_image_show);

static mp_obj_t py_image_width(mp_obj_t img_obj)
{
    return mp_obj_new_int(((image_t *)py_img_cobj(img_obj))->w);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_width_obj, py_image_width);

static mp_obj_t py_image_height(mp_obj_t img_obj)
{
    return mp_obj_new_int(((image_t *)py_img_cobj(img_obj))->h);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_height_obj, py_image_height);

static mp_obj_t py_image_format(mp_obj_t img_obj)
{
    switch (((image_t *)py_img_cobj(img_obj))->bpp)
    {
    case PIXFORMAT_INVALID:
        return mp_obj_new_str_via_qstr("PIXFORMAT_INVALID", 18 - 1);
    case PIXFORMAT_BINARY:
        return mp_obj_new_str_via_qstr("PIXFORMAT_BINARY", 17 - 1);
    case PIXFORMAT_GRAYSCALE:
        return mp_obj_new_str_via_qstr("PIXFORMAT_GRAYSCALE", 20 - 1);
    case PIXFORMAT_RGB565:
        return mp_obj_new_str_via_qstr("PIXFORMAT_RGB565", 17 - 1);
    case PIXFORMAT_YUV422:
        return mp_obj_new_str_via_qstr("PIXFORMAT_YUV422", 17 - 1);
    case PIXFORMAT_BAYER:
        return mp_obj_new_str_via_qstr("PIXFORMAT_BAYER", 16 - 1);
    case PIXFORMAT_RGB888_HWC:
        return mp_obj_new_str_via_qstr("PIXFORMAT_RGB888_HWC", 21 - 1);
    case PIXFORMAT_RGB888_CHW:
        return mp_obj_new_str_via_qstr("PIXFORMAT_RGB888_CHW", 21 - 1);
    default:
        return mp_obj_new_str_via_qstr("PIXFORMAT_JPEG", 15 - 1);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_format_obj, py_image_format);

static mp_obj_t py_image_size(mp_obj_t img_obj)
{
    return mp_obj_new_int(image_size((image_t *) py_img_cobj(img_obj)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_size_obj, py_image_size);

static mp_obj_t py_image_bytearray(mp_obj_t img_obj)
{
    image_t *arg_img = (image_t *) py_img_cobj(img_obj);
    return mp_obj_new_bytearray_by_ref(image_size(arg_img), arg_img->data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_bytearray_obj, py_image_bytearray);

STATIC mp_obj_t py_image_get_pixel(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_not_compressed(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 2, &arg_vec);
    int arg_x = mp_obj_get_int(arg_vec[0]);
    int arg_y = mp_obj_get_int(arg_vec[1]);

    bool arg_rgbtuple = py_helper_keyword_int(n_args, args, offset, kw_args,
            MP_OBJ_NEW_QSTR(MP_QSTR_rgbtuple), arg_img->pixfmt == PIXFORMAT_RGB565);

    if ((!IM_X_INSIDE(arg_img, arg_x)) || (!IM_Y_INSIDE(arg_img, arg_y))) {
        return mp_const_none;
    }

    switch (arg_img->pixfmt) {
        case PIXFORMAT_BINARY: {
            if (arg_rgbtuple) {
                int pixel = IMAGE_GET_BINARY_PIXEL(arg_img, arg_x, arg_y);
                mp_obj_t pixel_tuple[3];
                pixel_tuple[0] = mp_obj_new_int(COLOR_RGB565_TO_R8(COLOR_BINARY_TO_RGB565(pixel)));
                pixel_tuple[1] = mp_obj_new_int(COLOR_RGB565_TO_G8(COLOR_BINARY_TO_RGB565(pixel)));
                pixel_tuple[2] = mp_obj_new_int(COLOR_RGB565_TO_B8(COLOR_BINARY_TO_RGB565(pixel)));
                return mp_obj_new_tuple(3, pixel_tuple);
            } else {
                return mp_obj_new_int(IMAGE_GET_BINARY_PIXEL(arg_img, arg_x, arg_y));
            }
        }
        case PIXFORMAT_GRAYSCALE: {
            if (arg_rgbtuple) {
                int pixel = IMAGE_GET_GRAYSCALE_PIXEL(arg_img, arg_x, arg_y);
                mp_obj_t pixel_tuple[3];
                pixel_tuple[0] = mp_obj_new_int(COLOR_RGB565_TO_R8(COLOR_GRAYSCALE_TO_RGB565(pixel)));
                pixel_tuple[1] = mp_obj_new_int(COLOR_RGB565_TO_G8(COLOR_GRAYSCALE_TO_RGB565(pixel)));
                pixel_tuple[2] = mp_obj_new_int(COLOR_RGB565_TO_B8(COLOR_GRAYSCALE_TO_RGB565(pixel)));
                return mp_obj_new_tuple(3, pixel_tuple);
            } else {
                return mp_obj_new_int(IMAGE_GET_GRAYSCALE_PIXEL(arg_img, arg_x, arg_y));
            }
        }
        case PIXFORMAT_RGB565: {
            if (arg_rgbtuple) {
                int pixel = IMAGE_GET_RGB565_PIXEL(arg_img, arg_x, arg_y);
                mp_obj_t pixel_tuple[3];
                pixel_tuple[0] = mp_obj_new_int(COLOR_RGB565_TO_R8(pixel));
                pixel_tuple[1] = mp_obj_new_int(COLOR_RGB565_TO_G8(pixel));
                pixel_tuple[2] = mp_obj_new_int(COLOR_RGB565_TO_B8(pixel));
                return mp_obj_new_tuple(3, pixel_tuple);
            } else {
                return mp_obj_new_int(IMAGE_GET_RGB565_PIXEL(arg_img, arg_x, arg_y));
            }
        }
        case PIXFORMAT_BAYER_ANY:
            if (arg_rgbtuple) {
                uint16_t pixel; imlib_debayer_line(arg_x, arg_x + 1, arg_y, &pixel, PIXFORMAT_RGB565, arg_img);
                mp_obj_t pixel_tuple[3];
                pixel_tuple[0] = mp_obj_new_int(COLOR_RGB565_TO_R8(pixel));
                pixel_tuple[1] = mp_obj_new_int(COLOR_RGB565_TO_G8(pixel));
                pixel_tuple[2] = mp_obj_new_int(COLOR_RGB565_TO_B8(pixel));
                return mp_obj_new_tuple(3, pixel_tuple);
            } else {
                return mp_obj_new_int(IMAGE_GET_BAYER_PIXEL(arg_img, arg_x, arg_y));
            }
        case PIXFORMAT_YUV_ANY:
            if (arg_rgbtuple) {
                uint16_t pixel; imlib_deyuv_line(arg_x, arg_x + 1, arg_y, &pixel, PIXFORMAT_RGB565, arg_img);
                mp_obj_t pixel_tuple[3];
                pixel_tuple[0] = mp_obj_new_int(COLOR_RGB565_TO_R8(pixel));
                pixel_tuple[1] = mp_obj_new_int(COLOR_RGB565_TO_G8(pixel));
                pixel_tuple[2] = mp_obj_new_int(COLOR_RGB565_TO_B8(pixel));
                return mp_obj_new_tuple(3, pixel_tuple);
            } else {
                return mp_obj_new_int(IMAGE_GET_YUV_PIXEL(arg_img, arg_x, arg_y));
            }
        default: return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_get_pixel_obj, 2, py_image_get_pixel);

STATIC mp_obj_t py_image_set_pixel(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_not_compressed(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 2, &arg_vec);
    int arg_x = mp_obj_get_int(arg_vec[0]);
    int arg_y = mp_obj_get_int(arg_vec[1]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, offset, kw_args, -1); // White.

    if ((!IM_X_INSIDE(arg_img, arg_x)) || (!IM_Y_INSIDE(arg_img, arg_y))) {
        return args[0];
    }

    switch (arg_img->pixfmt) {
        case PIXFORMAT_BINARY: {
            IMAGE_PUT_BINARY_PIXEL(arg_img, arg_x, arg_y, arg_c);
            return args[0];
        }
        case PIXFORMAT_GRAYSCALE:
        case PIXFORMAT_BAYER_ANY: { // re-use
            IMAGE_PUT_GRAYSCALE_PIXEL(arg_img, arg_x, arg_y, arg_c);
            return args[0];
        }
        case PIXFORMAT_RGB565:
        case PIXFORMAT_YUV_ANY: { // re-use
            IMAGE_PUT_RGB565_PIXEL(arg_img, arg_x, arg_y, arg_c);
            return args[0];
        }
        default: return args[0];
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_set_pixel_obj, 2, py_image_set_pixel);

static mp_obj_t py_to_rgb565(mp_obj_t img_obj)
{
    image_t *ori = py_img_cobj(img_obj);
    if(ori->pixfmt==PIXFORMAT_RGB565){return img_obj;}
    if((ori->pixfmt==PIXFORMAT_GRAYSCALE)
    || (ori->pixfmt==PIXFORMAT_RGB888_CHW)
    || (ori->pixfmt==PIXFORMAT_RGB888_HWC))
    {}else{ mp_raise_TypeError("now just support GRAYSCALER/GB888_CHW/RGB888_HWC");}

    int size = ori->w * ori->h *2 ;
    void *pix = m_malloc(size);
    py_img_obj_t *img565_obj = py_img(ori->w, ori->h, PIXFORMAT_RGB565, size, pix);
    image_t *res = py_img_cobj(img565_obj);
    to_rgb565(ori, res);

    return MP_OBJ_FROM_PTR(img565_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_to_rgb565_obj, py_to_rgb565);

static mp_obj_t py_to_gray(mp_obj_t img_obj)
{
    image_t *ori = py_img_cobj(img_obj);
    if(ori->pixfmt==PIXFORMAT_GRAYSCALE){return img_obj;}
    if((ori->pixfmt==PIXFORMAT_RGB565)
    || (ori->pixfmt==PIXFORMAT_RGB888_CHW)
    || (ori->pixfmt==PIXFORMAT_RGB888_HWC))
    {}else{ mp_raise_TypeError("now just support RGB565/GB888_CHW/RGB888_HWC");}

    int size = ori->w * ori->h  ;
    void *pix = m_malloc(size);
    py_img_obj_t *img_gray_obj = py_img(ori->w, ori->h, PIXFORMAT_GRAYSCALE, size, pix);
    image_t *res = py_img_cobj(img_gray_obj);
    to_gray(ori, res);

    return MP_OBJ_FROM_PTR(img_gray_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_to_gray_obj, py_to_gray);

static mp_obj_t py_crop(size_t argc, const mp_obj_t *argv)
{
    enum{ARG_self_obj,
        ARG_offset_height,
        ARG_offset_width,
        ARG_height,
        ARG_width};

    image_t *ori = py_img_cobj(argv[ARG_self_obj]);
    if((ori->pixfmt==PIXFORMAT_RGB565)
    || (ori->pixfmt==PIXFORMAT_GRAYSCALE)
    || (ori->pixfmt==PIXFORMAT_RGB888_CHW)
    || (ori->pixfmt==PIXFORMAT_RGB888_HWC))
    {}else{ mp_raise_TypeError("now just support RGB565/GRAYSCALER/GB888_CHW/RGB888_HWC");}

    int oh = mp_obj_get_int(argv[ARG_offset_height]);
    int ow = mp_obj_get_int(argv[ARG_offset_width]);
    int res_h = mp_obj_get_int(argv[ARG_height]);
    int res_w = mp_obj_get_int(argv[ARG_width]);

    int size = res_h * res_w * (ori->bpp) ;
    void *pix = m_malloc(size);
    py_img_obj_t *res_obj = py_img(res_w, res_h, ori->pixfmt, size, pix);
    image_t *res = py_img_cobj(res_obj);
    crop(ori, res, oh, ow);;
    return MP_OBJ_FROM_PTR(res_obj);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_crop_obj, 5, 5, py_crop);

static mp_obj_t py_resize(const mp_obj_t self, const mp_obj_t obj_w, const mp_obj_t obj_h)
{
    image_t *ori = py_img_cobj(self);
    if((ori->pixfmt==PIXFORMAT_RGB565)
    || (ori->pixfmt==PIXFORMAT_GRAYSCALE)
    || (ori->pixfmt==PIXFORMAT_RGB888_CHW)
    || (ori->pixfmt==PIXFORMAT_RGB888_HWC))
    {}else{ mp_raise_TypeError("now just support RGB565/GRAYSCALER/GB888_CHW/RGB888_HWC");}

    int res_w = mp_obj_get_int(obj_w);
    int res_h = mp_obj_get_int(obj_h);

    int size = res_w * res_h * (ori->bpp) ;
    void *pix = m_malloc(size);
    py_img_obj_t *res_obj = py_img(res_w, res_h, ori->pixfmt, size, pix);
    image_t *res = py_img_cobj(res_obj);
    resize_bilinera_interpolation(ori, res);
    return MP_OBJ_FROM_PTR(res_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(py_resize_obj, py_resize);

STATIC mp_obj_t make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 0, 4, true);
    mp_obj_type_t *img_obj;
    if (n_args == 0)
    {
        img_obj = py_img(0, 0, 0, 0, 0);
    }
    else if (n_args == 4)
    {
        mp_buffer_info_t bufinfo;
        if (mp_get_buffer(args[0], &bufinfo, MP_BUFFER_READ)){
            int w = mp_obj_get_int(args[2]);
            int h = mp_obj_get_int(args[3]);
            int pixfmt = mp_obj_get_int(args[1]);

            img_obj = py_img(w, h, (pixformat_t)pixfmt, bufinfo.len, bufinfo.buf);
        }else{mp_raise_ValueError("get image fail!");}
        
    }
    else
    {
        mp_raise_TypeError("use: (data, pixformat, w, h)");
    }

    return MP_OBJ_FROM_PTR(img_obj);
}

STATIC const mp_rom_map_elem_t locals_dict_table[] =
    {
        {MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&py_image_show_obj)},

        /* Basic Methods */
        {MP_ROM_QSTR(MP_QSTR_width),    MP_ROM_PTR(&py_image_width_obj)},
        {MP_ROM_QSTR(MP_QSTR_height),   MP_ROM_PTR(&py_image_height_obj)},
        {MP_ROM_QSTR(MP_QSTR_format),   MP_ROM_PTR(&py_image_format_obj)},
        {MP_ROM_QSTR(MP_QSTR_size),        MP_ROM_PTR(&py_image_size_obj)},
//        {MP_ROM_QSTR(MP_QSTR_bytearray),   MP_ROM_PTR(&py_image_bytearray_obj)},
        {MP_ROM_QSTR(MP_QSTR_get_pixel),   MP_ROM_PTR(&py_image_get_pixel_obj)},
        {MP_ROM_QSTR(MP_QSTR_set_pixel),   MP_ROM_PTR(&py_image_set_pixel_obj)},
        {MP_ROM_QSTR(MP_QSTR_to_rgb565),   MP_ROM_PTR(&py_to_rgb565_obj)},
        {MP_ROM_QSTR(MP_QSTR_to_gray),   MP_ROM_PTR(&py_to_gray_obj)},
        {MP_ROM_QSTR(MP_QSTR_resize),      MP_ROM_PTR(&py_resize_obj)},
        {MP_ROM_QSTR(MP_QSTR_crop),        MP_ROM_PTR(&py_crop_obj)},
        {MP_ROM_QSTR(MP_QSTR_PIXFORMAT_INVALID),    MP_ROM_INT(PIXFORMAT_INVALID)},
        {MP_ROM_QSTR(MP_QSTR_PIXFORMAT_BINARY),     MP_ROM_INT(PIXFORMAT_BINARY)},
        {MP_ROM_QSTR(MP_QSTR_PIXFORMAT_GRAYSCALE),  MP_ROM_INT(PIXFORMAT_GRAYSCALE)},
        {MP_ROM_QSTR(MP_QSTR_PIXFORMAT_RGB565),     MP_ROM_INT(PIXFORMAT_RGB565)},
        {MP_ROM_QSTR(MP_QSTR_PIXFORMAT_YUV422),     MP_ROM_INT(PIXFORMAT_YUV422)},
        {MP_ROM_QSTR(MP_QSTR_PIXFORMAT_BAYER),      MP_ROM_INT(PIXFORMAT_BAYER)},
        {MP_ROM_QSTR(MP_QSTR_PIXFORMAT_RGB888_HWC), MP_ROM_INT(PIXFORMAT_RGB888_HWC)},
        {MP_ROM_QSTR(MP_QSTR_PIXFORMAT_RGB888_CHW), MP_ROM_INT(PIXFORMAT_RGB888_CHW)},
        {MP_ROM_QSTR(MP_QSTR_PIXFORMAT_JPEG),       MP_ROM_INT(PIXFORMAT_JPEG)},
};
STATIC MP_DEFINE_CONST_DICT(locals_dict, locals_dict_table);

static void py_image_print(const mp_print_t *print, mp_obj_t self, mp_print_kind_t kind)
{
    image_t *image = py_img_cobj(self);
    if (image->pixfmt == PIXFORMAT_JPEG
            && image->pixels[0] == 0xFE
            && image->pixels[image->size-1] == 0xFE) {
        // print for ide.
        print->print_strn(print->data, (const char *) image->pixels, image->size);
    } else {
        mp_printf(print, "{\"w\":%d, \"h\":%d, \"type\":\"%s\", \"size\":%d}",
                image->w,
                image->h,
                (image->pixfmt == PIXFORMAT_BINARY)     ? "binary" :
                (image->pixfmt == PIXFORMAT_GRAYSCALE)  ? "grayscale" :
                (image->pixfmt == PIXFORMAT_RGB565)     ? "rgb565" :
                (image->pixfmt == PIXFORMAT_BAYER_BGGR) ? "bayer_bggr" :
                (image->pixfmt == PIXFORMAT_BAYER_GBRG) ? "bayer_gbrg" :
                (image->pixfmt == PIXFORMAT_BAYER_GRBG) ? "bayer_grbg" :
                (image->pixfmt == PIXFORMAT_BAYER_RGGB) ? "bayer_rggb" :
                (image->pixfmt == PIXFORMAT_YUV422)     ? "yuv422" :
                (image->pixfmt == PIXFORMAT_YVU422)     ? "yvu422" :
                (image->pixfmt == PIXFORMAT_JPEG)       ? "jpeg" : 
                (image->pixfmt == PIXFORMAT_RGB888_HWC) ? "rgb888_hwc" :
                (image->pixfmt == PIXFORMAT_RGB888_CHW) ? "rgb888_chw" : "unknown",
                image_size(image));
    }
}

STATIC mp_int_t py_image_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags)
{
    py_img_obj_t *image_obj = (py_img_obj_t *)obj;
    image_t *pic = py_img_cobj(image_obj);
    switch (flags)
    {
    case MP_BUFFER_READ:
        bufinfo->buf = pic->pixels;
        bufinfo->len = pic->w * pic->h * (pic->bpp);
        bufinfo->typecode = 'b';
        break;
    case MP_BUFFER_WRITE:
        image_obj->_cobj.pixels = bufinfo->buf;
        break;
    case MP_BUFFER_RW:
        /* code */
        break;
    default:
        break;
    }
    return 0;
}

const mp_obj_type_t py_img_type =
    {
        {&mp_type_type},
        .name = MP_QSTR_Image,
        .print = py_image_print,
        .make_new = make_new,
        .locals_dict = (mp_obj_t)&locals_dict,
        .buffer_p = {.get_buffer = py_image_get_buffer}};
#endif
