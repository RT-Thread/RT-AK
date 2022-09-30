# Micropython使用指南

MicroPython 是 Python 3 编程语言的一种精简而高效的实现，它包含 Python 标准库的一个子集，并被优化为在微控制器和受限环境中运行。

本文档是基于[RT-Thread官方文档]([介绍 (rt-thread.org)](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/packages-manual/micropython-docs/introduction))对Draco开发板上MicroPython的一个补充说明文档, 具体请参考RT-Thread官方MicroPython文档.

# MicroPython特定库

Micropython特定库是指和芯片具体硬件设备相关的控制模块.  MicropPython特定库对芯片常见的通用设备的调用, 其底层对接到RT-Thread操作系统设备框架层. 因此MicroPython对设备查找和初始化时所使用的设备名与RT-Thread设备框架中注册的设备名相同, 即可在工程的driver文件夹下的设备驱动源码中查看注册的设备名. 或者在RT-Thread系统下`msh>` 命令行中使用`list_device` 命令查看注册的设备. 

注意: 要使用的设备也须现在Draco工程的`RT-Thread Settings`中开启对应设备.

## machine模块

machine模块为MCU芯片常见设备控制模块.

### machine.Pin

在创建`Pin`设备类对象时, 传入的`id`编号即是原理图中`IO0~IO47`引脚编号对应.

```python
from machine import Pin
# 查看Pin类成员
dir(Pin)
# machine.Pin( id, mode = -1, pull = -1，value)
# id: ('name', num) name为用户引脚别名, num为引脚编号(和原理图IO0-IO47对应).
pin30 = Pin(('rgb_r',30), mode=Pin.OUT_PP, value=1)
pin38 = Pin(('rgb_r',38), mode=Pin.OUT_PP, value=1)
pin39 = Pin(('rgb_g',39), mode=Pin.OUT_PP, value=1)
pin40 = Pin(('rgb_b',40), mode=Pin.OUT_PP, value=1)
# light led
pin30.value(0)
pin38.value(0)
pin39.value(0)
pin40.value(0)

```

### machine.I2C

I2C设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板可注册的I2C设备有: `i2c0`, `i2c1`, `i2c2`. 因此使用时可在`machine.I2C(id= -1, scl, sda, freq=400000)` 的id参数中传入`0`, `1`, `2`. 

### machine.SPI

SPI设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板上可注册的有`spi1`总线, 同时可配置4个spi片选设备, 设备名为spi10, spi11, spi12, spi13, 因此可在`class machine.SPI(id, ...)`的id参数中传入如`10`, `11`, `12`, `13`.

### machine.UART

UART设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板上可注册的有3个UART总线, 设备名为uart1, uart2, uart3, 因此可在`class machine.UART(id, ...)` 的id参数中可传入如`1`, `2`, `3`.

### machine.LCD

LCD设备模块使用方法参考RT-Thread官方MicroPython文档.

### machine.RTC

RTC设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板注册了一个设备名为"rtc"的rtc设备.

### machine.PWM

PWM设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板上仅有一个pwm模块, 设备名为`"pwm"`, 因此在`class machine.PWM(id, channel, freq, duty)` 的id参数需要传入`"pwm"`设备名来查找和初始化设备.

### machine.ADC

**K210芯片没有ADC设备**

### machine.WDT

WDT设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板上可注册2个, 设备名为"wdt0"和"wdt1"的设备, 因此在`class machine.WDT(id , timeout)` 的id参数可传入`"wdt0"`, `"wdt1"`或者 `0`, `1`来查找和使用wdt设备.

### machine.TIMER

Timer设备模块主要使用方法参考RT-Thread官方MicroPython文档. Draco开发板上通过`driver/drv_hw_timer` 可以看到注册的Timer设备有`timer00`, `timer01`, `timer02`, `timer03` `timer10`, `timer11`, `timer12`, `timer13` 其中命名中的数字分别代表设备号和通道号, 例如`timer01` 代表timer的0号设备的1通道.

## k210模块

k210模块为k210芯片上搭载的特定模块的控制模块.

### k210.FPIOA

k210芯片上提供的一些特定设备都放在k210模块下. 

> FPIOA（现场可编程 IO 阵列）允许用户将 255 个内部功能映射到芯片外围的 48 个自由 IO 上。主要功能:
>
> • 支持 IO 的可编程功能选择
>
> • 支持 IO 输出的 8 种驱动能力选择
>
> • 支持 IO 的内部上拉电阻选择
>
> • 支持 IO 的内部下拉电阻选择
>
> • 支持 IO 输入的内部施密特触发器设置
>
> • 支持 IO 输出的斜率控制
>
> • 支持内部输入逻辑的电平设置

**构造函数** 

`class k210.FPIOA()` 

**方法** 

`bool set_function(int pin, int func, int set_sl, int set_st,int set_io_driving)` 

设置引脚功能 

- pin : 引脚号，取值范围 0 到 48 

- func : 引脚功能，取值范围见下表, 可使用`dir(FPIOA)`查看该类成员函数和可配置功能.

- set_sl : （可选参数） 

- set_st : （可选参数） 

- set_io_driving : （可选参数） 

`int get_io_by_function(int func)` 

查看某功能所在引脚

- func : 取值范围如下:

```
['JTAG_TCLK', 'JTAG_TDI', 'JTAG_TMS', 'JTAG_TDO', 'SPI0_D0', 'SPI0_D1', 'SPI0_D2', 
'SPI0_D3', 'SPI0_D4', 'SPI0_D5', 'SPI0_D6', 'SPI0_D7', 'SPI0_SS0', 'SPI0_SS1', 
'SPI0_SS2', 'SPI0_SS3', 'SPI0_ARB', 'SPI0_SCLK', 'UARTHS_RX', 'UARTHS_TX', 'CLK_SPI1',
'CLK_I2C1', 'GPIOHS0', 'GPIOHS1', 'GPIOHS2', 'GPIOHS3', 'GPIOHS4', 'GPIOHS5', 
'GPIOHS6', 'GPIOHS7', 'GPIOHS8', 'GPIOHS9', 'GPIOHS10', 'GPIOHS11', 'GPIOHS12', 
'GPIOHS13', 'GPIOHS14', 'GPIOHS15', 'GPIOHS16', 'GPIOHS17', 'GPIOHS18', 'GPIOHS19', 
'GPIOHS20', 'GPIOHS21', 'GPIOHS22', 'GPIOHS23', 'GPIOHS24', 'GPIOHS25', 'GPIOHS26', 
'GPIOHS27', 'GPIOHS28', 'GPIOHS29', 'GPIOHS30', 'GPIOHS31', 'GPIO0', 'GPIO1', 'GPIO2', 
'GPIO3', 'GPIO4', 'GPIO5', 'GPIO6', 'GPIO7', 'UART1_RX', 'UART1_TX', 'UART2_RX', 
'UART2_TX', 'UART3_RX', 'UART3_TX', 'SPI1_D0', 'SPI1_D1', 'SPI1_D2', 'SPI1_D3', 
'SPI1_D4', 'SPI1_D5', 'SPI1_D6', 'SPI1_D7', 'SPI1_SS0', 'SPI1_SS1', 'SPI1_SS2', 
'SPI1_SS3', 'SPI1_ARB', 'SPI1_SCLK', 'SPI_SLAVE_D0', 'SPI_SLAVE_SS', 
'SPI_SLAVE_SCLK', 'I2S0_MCLK', 'I2S0_SCLK', 'I2S0_WS', 'I2S0_IN_D0', 'I2S0_IN_D1', 
'I2S0_IN_D2', 'I2S0_IN_D3', 'I2S0_OUT_D0', 'I2S0_OUT_D1', 'I2S0_OUT_D2', 
'I2S0_OUT_D3', 'I2S1_MCLK', 'I2S1_SCLK', 'I2S1_WS', 'I2S1_IN_D0', 'I2S1_IN_D1', 
'I2S1_IN_D2', 'I2S1_IN_D3', 'I2S1_OUT_D0', 'I2S1_OUT_D1', 'I2S1_OUT_D2', 
'I2S1_OUT_D3', 'I2S2_MCLK', 'I2S2_SCLK', 'I2S2_WS', 'I2S2_IN_D0', 'I2S2_IN_D1', 
'I2S2_IN_D2', 'I2S2_IN_D3', 'I2S2_OUT_D0', 'I2S2_OUT_D1', 'I2S2_OUT_D2', 
'I2S2_OUT_D3', 'I2C0_SCLK', 'I2C0_SDA', 'I2C1_SCLK', 'I2C1_SDA', 'I2C2_SCLK', 
'I2C2_SDA', 'CMOS_XCLK', 'CMOS_RST', 'CMOS_PWDN', 'CMOS_VSYNC', 'CMOS_HREF', 
'CMOS_PCLK', 'CMOS_D0', 'CMOS_D1', 'CMOS_D2', 'CMOS_D3', 'CMOS_D4', 'CMOS_D5', 
'CMOS_D6', 'CMOS_D7', 'SCCB_SCLK', 'SCCB_SDA', 'UART1_CTS', 'UART1_DSR', 'UART1_DCD', 
'UART1_RI', 'UART1_SIR_IN', 'UART1_DTR', 'UART1_RTS', 'UART1_OUT2', 'UART1_OUT1', 
'UART1_SIR_OUT', 'UART1_BAUD', 'UART1_RE', 'UART1_DE', 'UART1_RS485_EN', 'UART2_CTS', 
'UART2_DSR', 'UART2_DCD', 'UART2_RI', 'UART2_SIR_IN', 'UART2_DTR', 'UART2_RTS',
'UART2_OUT2', 'UART2_OUT1', 'UART2_SIR_OUT', 'UART2_BAUD', 'UART2_RE', 'UART2_DE', 
'UART2_RS485_EN', 'UART3_CTS', 'UART3_DSR', 'UART3_DCD', 'UART3_RI', 'UART3_SIR_IN', 
'UART3_DTR', 'UART3_RTS', 'UART3_OUT2', 'UART3_OUT1', 'UART3_SIR_OUT', 'UART3_BAUD', 
'UART3_RE', 'UART3_DE', 'UART3_RS485_EN', 'TIMER0_TOGGLE1', 'TIMER0_TOGGLE2', 
'TIMER0_TOGGLE3', 'TIMER0_TOGGLE4', 'TIMER1_TOGGLE1', 'TIMER1_TOGGLE2', 
'TIMER1_TOGGLE3', 'TIMER1_TOGGLE4', 'TIMER2_TOGGLE1', 'TIMER2_TOGGLE2', 
'TIMER2_TOGGLE3', 'TIMER2_TOGGLE4', 'CLK_SPI2', 'CLK_I2C2', 'DRIVING_0', 'DRIVING_1', 
'DRIVING_2', 'DRIVING_3', 'DRIVING_4', 'DRIVING_5', 'DRIVING_6', 'DRIVING_7', 
'DRIVING_8', 'DRIVING_9', 'DRIVING_10', 'DRIVING_11', 'DRIVING_12', 'DRIVING_13', 
'DRIVING_14', 'DRIVING_15']
```

**例程:**

FPIOA+PWM配置例程:

```python
from k210 import FPIOA
from machine import PWM

fpioa = FPIOA() 
# 查看fpio的方法和可选功能映射
dir(fpioa)
# TIMER2_TOGGLE1-4对应PWM0-3通道
# 设置30号IO引脚(LED)功能映射为TIMER2_TOGGLE1(PWM通道0), 
fpioa.set_function(30, fpioa.TIMER2_TOGGLE1)
pwm=PWM('pwm',0,1000,250)
pwm.init(0,1000,150)
pwm.init(0,1000,0)   
```

### k210.I2S

**构造函数** 

`class k210.I2S(int bus, int mode)` 

- bus : I2S 总线编号，取值范围 0 到 2 

- mode : 收/发模式，取值 TRANSMITTER(播放)，RECEIVER(录音) 

**方法** 

`void init()` 

初始化 

`void set_param(int sample_rate, int bps, int track_num)` 

设置参数 

- sample_rate: 采样率 

- bps : 采样位数 

- track_num : 音道数，取值 1 或 2 

`void play(bytearray pcm)` 

播放

- pcm : PCM 格式的音频数据 

`bytearray record(void)` 

录音 

**示例** 

参考工程目录`samples/python`路径下示例

### k210.FFT

**构造函数** 

`class k210.FFT()` 

**方法** 

`list<int real, int imag> run(int[] input, int shift, int direction)` 

计算 fft 

参数说明： 

- input : 待变换的数据，类型：int 数组 

- shift : 默认 0 

- direction : 取值 DIR_BACKWARD/DIR_FORWARD 

返回值说明： 

- return : 返回包含实部和虚部的列表，虚/实为 int 类型

### k210.camera

**构造函数** 

`class k210.camera()` 

**方法** 

`void reset()` 

初始化 

`void set_pixformat(int fmt)` 

设置图像格式 

-- fmt : 格式，取值 RGB565 

`void set_framesize(int width, int height)` 

设置图像大小 

`snapshot()` 

采集图像, **该函数在K210上将返回一个`list` 包含2副图像`[Image_rgb565, Image_rgb888]`** 

**注意:** 当开启OpenMV时, 该类将失效. 将使用`sensor`类代替. camera方法返回的Image类仅具有少量图像处理方法, 和OpenMV中的Image并非同一个类.

### k210.Image

k210.Image为摄像头采集返回的图像格式, 包含了针对k210定制的相关处理函数. 主要用于`RGB565` (用于LCD显示) 和`RGB888 CHW`(用于AI输入) 格式的处理和转换. 

**注意:** 该Image与**OpenMV**中的Image对象不同. 在Draco开发板上当使能了OpenMV时, 将自动禁用该类. 转而使用OpenMV的图像处理库.

#### image.width()

返回以像素计的图像的宽度。

#### image.height()

返回以像素计的图像的高度。

#### image.format()

返回用于灰度图的 `sensor.GRAYSCALE` 、用于RGB图像的 `sensor.RGB565` 和用于JPEG图像的 `sensor.JPEG` 。

#### image.size()

返回以字节计的图像大小。

#### image.get_pixel(x, y[, rgbtuple])

灰度图：返回(x, y)位置的灰度像素值。

RGB565l：返回(x, y)位置的RGB888像素元组(r, g, b)。

Bayer图像: 返回(x, y)位置的像素值。

不支持压缩图像。

#### image.set_pixel(x, y, pixel)

灰度图：将`(x, y)` 位置的像素设置为灰度值 pixel 。 RGB图像：将`(x, y)` 位置的像素设置为RGB888元组`(r, g, b)` pixel 。 对于bayer模式图像:将位置`(x, y)`的像素值设置为 pixel。

返回image对象，这样你可以使用 . 符号调用另一个方法。

x 和 y 可以独立传递，也可以作为元组传递。

pixel 可以是 `RGB888` 元组 (r, g, b) 或底层像素值（即 RGB565 图像的字节反转 RGB565 值或灰度图像的 8 位值。

不支持压缩图像。

#### image.to_rgb565()

用于将图片转换为rgb565格式. 该方法会返回新的图像buffer.

#### image.to_gray()

将图片转换为灰度图格式. 该方法会返回新的图像buffer.

#### image.resize(w, h)

缩放图片宽高为`w` 和`h` . 该方法会返回新的图像buffer.

#### image.crop(y, x, offset_h, offset_w)

从`image`的`(x, y)`起始位置裁剪图片, 裁剪大小为`offset_h`和`offset_w`.

# OpenMV模块

Draco开发板上移植适配了OpenMV图像处理库, OpenMV使用参考: 

[OpenMV图像处理的方法 · OpenMV中文入门教程](https://book.openmv.cc/image/)

## Draco开发板OpenMV使用注意事项

* OpenMV中自带了摄像头驱动, 在开启OpenMV时, 需要将RT-Thread Settings中`硬件 ->Enable Camera -> Select camera type` 配置选择为`Camera using other` 选项, 关闭`driver` 下的摄像头驱动. 否则会报重复定义的错误.
* 备注: K210上OpenMV`snapshot()`函数 **该函数在K210上将返回一个`list` 包含2副图像`[Image_rgb565, Image_rgb888]`** 如下:

```python
img1, img2 = sensor.snapshot()
# img1为RGB565用于LCD显示, img2位RGB888用于AI输入
```

