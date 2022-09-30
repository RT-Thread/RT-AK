#ifndef __RT_AI_IMAGE__
#define __RT_AI_IAMGE__

#include <stdlib.h>
#include <imlib.h>
/**
 * @brief bilinera interpolation resize. now support PIXFORMAT_GRAYSCALE/PIXFORMAT_RGB565/PIXFORMAT_RGB888_CHW/PIXFORMAT_RGB888_HWC
 * 
 * @param ori origin image
 * @param res result image
 */
void resize_bilinera_interpolation(image_t *ori, image_t *res);

/**
 * @brief crop ori img to res img by offset. now support PIXFORMAT_GRAYSCALE/PIXFORMAT_RGB565/PIXFORMAT_RGB888_CHW/PIXFORMAT_RGB888_HWC
 * @param ori origin image
 * @param res result image
 * @param offset_height height offset with start to crop
 * @param offset_width width offset with start to crop
 */
void crop(image_t *ori, image_t *res, uint32_t offset_height, uint32_t offset_width);

/**
 * @brief covert ori img to grayscale to res. now support PIXFORMAT_GRAYSCALE/PIXFORMAT_RGB565/PIXFORMAT_RGB888_CHW/PIXFORMAT_RGB888_HWC
 * 
 * @param ori origin image
 * @param res result image
 */
void to_gray(image_t *ori, image_t *res);

/**
 * @brief covert to ori img to rgb565 to res. now support PIXFORMAT_GRAYSCALE/PIXFORMAT_RGB565/PIXFORMAT_RGB888_CHW/PIXFORMAT_RGB888_HWC
 * 
 * @param ori origin image
 * @param res result image
 */
void to_rgb565(image_t *ori, image_t *res);

/**
 * @brief Set the pixel object. now support PIXFORMAT_GRAYSCALE/PIXFORMAT_RGB565/PIXFORMAT_BINARY
 * 
 * @param img the image be set
 * @param x x
 * @param y y
 * @param p pixel
 */
void set_pixel(image_t *img, int x, int y, int p);

/**
 * @brief draw line. now support PIXFORMAT_GRAYSCALE/PIXFORMAT_RGB565/PIXFORMAT_BINARY
 * 
 * @param img image_t.
 * @param x0 start x.
 * @param y0 start y.
 * @param x1 end x.
 * @param y1 end y.
 * @param c color.
 * @param thickness thickness.
 */
void draw_line(image_t *img, int x0, int y0, int x1, int y1, int c, int thickness);

/**
 * @brief draw rectangle. now support PIXFORMAT_GRAYSCALE/PIXFORMAT_RGB565/PIXFORMAT_BINARY.
 * 
 * @param img image_t
 * @param rx right-up x.
 * @param ry right-up y.
 * @param rw width.
 * @param rh height.
 * @param c color.
 * @param thickness thickness. 
 * @param fill fill the rectangle.
 */
void draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c, int thickness, bool fill);

/**
 * @brief draw circle. now support PIXFORMAT_GRAYSCALE/PIXFORMAT_RGB565/PIXFORMAT_BINARY.
 * 
 * @param img image_t.
 * @param cx x of the centre of the circle.
 * @param cy y of the centre of a circle
 * @param r radius
 * @param c color
 * @param thickness thickness
 * @param fill fill the circle.
 */
void draw_circle(image_t *img, int cx, int cy, int r, int c, int thickness, bool fill);

/**
 * @brief draw string. now support PIXFORMAT_GRAYSCALE/PIXFORMAT_RGB565/PIXFORMAT_BINARY.
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
void draw_string(image_t *img, int x_off, int y_off, const char *str, int c, float scale, int x_spacing, int y_spacing, bool mono_space,
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
void rotation_corr(image_t *img, float x_rotation, float y_rotation, float z_rotation,
                         float x_translation, float y_translation,
                         float zoom, float fov, float *corners);

#endif //__RT_AI_IMAGE__
