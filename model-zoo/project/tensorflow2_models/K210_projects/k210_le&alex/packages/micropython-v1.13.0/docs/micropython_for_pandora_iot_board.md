# MicroPython for Pandora IoT Board

![IoT_Board](assets/IoT_Board.png)

[**IoT Board 潘多拉**](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-18400369818.12.2ba47ea5PzJxZx&id=583843059625) 是 RT-Thread 推出的一款物联网开发板，它给开发者带来了物联网时代的无限可能。而现在，它已经不仅仅是一块简单的物联网开发板，因为它已经全面支持 **MicroPython** 。在 IoT Board 上，你将会体验到有别于传统的，前所未有的开发方式。

借助于 MicroPython，你将有能力使用 Python 语言控制所有硬件外设，体验高级语言带来的便利特性，与此同时你还可以利用高级软件库快速实现你的 IoT 构想。

## 硬件支持

Pandora MicroPython 固件硬件功能如下所示：

| 外设名称 | 引脚号                                         | 简介                                      |
| -------- | ---------------------------------------------- | ----------------------------------------- |
| pin      | PA4 PA8, PB8-9 PB10-15, PC2 PC4 PC6-7, PD12-15 | 开发板引出的可自由分配的 IO，支持引脚中断 |
| led      | PE7                                            | 红色 led 灯                               |
| rgb      | R: PE7, G: PE8, B: PE9                         | rgb 灯                                    |
| key      | KEY0: PD10, KEY1: PD9, KEY2: PD8               | 输入按键                                  |
| uart1    | PA9, PA10                                      | 串口1                                     |
| i2c      |                                                | 软件 i2c 可选择任意 pin                   |
| spi      |                                                | 软件 spi 可选择任意引出 pin               |
| adc      | PC4                                            | adc1，通道 13                             |
| pwm      | PB0                                            | pwm3,  通道 3,  用于红外发射              |
| timer    |                                                | 硬件定时器 15                             |
| wdt      |                                                | 看门狗                                    |
| rtc      |                                                | 实时时钟                                  |
| beeper   | PB2                                            | 蜂鸣器                                    |
| lcd      |                                                | lcd 显示屏                                |
| wifi     |                                                | wifi 网络连接                             |
| aht10    | CLK: PD6, SDA: PC1                             | 温湿度传感器                              |
| ap3216c  | CLK: PC0, SDA: PC1                             | 接近与光强传感器                          |
| icm20608 | CLK: PC0, SDA: PC1                             | 六轴传感器                                |


## 入门必读

如果您从来没有了解过 MicroPython, 可以阅读这篇简短的文章来 [带你入门 MicroPython](introduction.md)。

## 开启 MicroPython 之旅

推荐遵循如下步骤开始进行 MicroPython 开发：

- 在您的开发板上烧录合适的固件
- 在 PC 机上安装 RT-Thread MicroPython 开发环境并连接上开发板  

接下来就可以尽情挥洒您的创意了，更详细的内容可以点击下文中的链接来进一步了解。

### 下载合适的固件

- [Pandora IoT Board firmware](https://www.rt-thread.org/qa/forum.php?mod=viewthread&tid=12305&extra=page%3D1%26filter%3Dtypeid%26typeid%3D20)

### 安装 IDE 开发环境

- [RT-Thread MicroPython develop environment](https://marketplace.visualstudio.com/items?itemName=RT-Thread.rt-thread-micropython)

## 开发资料

### 示例程序

以下示例程序可以在 RT-Thread MicroPython IDE 开发环境中直接添加到工程：

![check_pandora_examples](assets/check_pandora_examples.png)

### MicroPython 模块详解

- [MicroPython Librarys](micropython-librarys.md)


## 联系我们

如果在使用的过程中遇到问题，您可以用如下方式联系我们：

- 在 github 上提交 issue
- [RT-Thread MicroPython 官方论坛](https://www.rt-thread.org/qa/forum.php?mod=forumdisplay&fid=2&filter=typeid&typeid=20)

- RT-Thread MicroPython 交流 QQ 群：703840633