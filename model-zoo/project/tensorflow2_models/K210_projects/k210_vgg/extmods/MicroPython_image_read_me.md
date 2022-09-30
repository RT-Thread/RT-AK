# RT-AK MicroPython 图像处理模块

## 1.简介

该目录为针对[RT-AK](https://github.com/RT-Thread/RT-AK)的MicroPython图像处理模块Image Lib适配. 建议用户可先对C语言接口使用方法有所了解.

## 2.RT-AK MicroPython函数使用说明

### K210 图像处理模块

K210 图像处理模块的所有python函数包含在extmods-k210文件夹中, 使用时应`import k210`. 在python环境下使用`dir(k210)`语句可查看模块所包含的函数名词.

### cam_reset()

该函数功能为初始化摄像头

使用示例：cam.reset()

### cam_set_framesize（height，weight）

该函数功能为设定截取的一帧图像分辨率大小，Darco开发板配置摄像头分辨率为320x240

使用示例：cam.set_framesize(320,240)

### cam_snapshot（）

该函数功能为摄像头截取当前图像，截取图像为两张，分别为RGB565格式与RGB888格式，可将其分别显示

使用示例：img1,img2=cam.snapshot()   //将截取的两张图像分别命名为img1、img2

​         lcd.show_image(0,0,240,320,img1)   //显示img1 

### crop（offset_height，offset_width，height，width）

该函数功能为图像的裁剪，可适用于RGB565与RGB888格式，offset_height，offset_width分别表示裁剪的起始坐标的高与宽，height，width分别表示所需裁剪的大小的长与宽

使用示例：new_img=img1.crop(0,0,160,160) // 将img1从（0，0）处开始裁剪160x160大小的图片并将裁剪的图片命名为new_img

### resize（height，width）

该函数的功能为图像的缩放，由于硬件固定了每一帧的分辨率为320x240，我们只需输入需要缩放的图像的高：height与宽：width，即可将图像缩放至指定大小

使用示例：new_img=img1.resize(160,160) // 将img1缩放至160x160大小，并将裁剪的图片命名为 new_img

### to_rgb565(name)

由于LCD显示屏只能显示RGB565格式的图像，故如果直接显示img2（RGB888）则会花屏，故我们在对img2裁剪与缩放后，都需要将其转换为RGB565格式的图片进行显示，该函数就是用于实现图片格式转换为RGB565的功能

 使用示例： new_img=img2.crop(0,0,160,160)

​           new_img1=new_img.to_rgb565() //将裁剪后的 new_img 转换为RGB565格式

​           lcd.show_image(0,0,160,160,new_img1)



### to_gray(name)

#### ***！！！由于LCD显示屏只能显示RGB565格式的图像，如果只需要灰度图像对应的数组，不需要显示，则可直接使用。如需显示灰度图像，需在转换后用to_rgb565转换成RGB565格式图像再显示，否则会报错！！！

该功能为了适配某些历程，可将RGB565、RGB888转换为grayscale格式

 使用示例：new_img=img1.to_gray() //将裁剪后的 new_img 转换为grayscale格式

​          new_img_gray=new_img.to_rgb565()//为了显示，将grayscale格式转换为RGB565格式

​          lcd.show_image(0,0,240,320,new_img_gray)

