# MicroPython for sparrow one board

![sparrow_one](assets/sparrow_one_board.png)

[**麻雀一号开发板**](https://item.taobao.com/item.htm?spm=a1z10.1-c.w4004-5210898174.2.29401ae39JyGKY&id=606684373403) 是 RT-Thread 推出的一款物联网音视频开发板，现在它已经全面支持 **MicroPython** 。在麻雀一号开发板上，你将会体验到有别于传统的，前所未有的开发方式。

借助于 MicroPython，你将有能力使用 Python 语言控制所有硬件外设，体验高级语言带来的便利特性，与此同时你还可以利用高级软件库快速实现你的 IoT 构想。

## 硬件支持

麻雀一号开发板固件硬件功能如下所示：

| 外设名称  | 简介                                                       |
| --------- | ---------------------------------------------------------- |
| key       | 输入按键                                                   |
| uart1     | 串口1                                                      |
| lcd       | lcd 显示屏                                                 |
| wifi      | wifi 网络连接                                              |
| bluetooth | 蓝牙                                                       |
| player    | 扬声器，音频播放                                           |
| recorder  | 麦克风，录音功能                                           |
| camera    | 摄像头，可拍照并存入文件系统，开启 TCP Server 查看实时图像 |

## 入门必读

如果您从来没有了解过 MicroPython, 可以阅读这篇简短的文章来 [带你入门 MicroPython](https://github.com/RT-Thread-packages/micropython/blob/master/docs/introduction.md)。

## 开启 MicroPython 之旅

推荐遵循如下步骤开始进行 MicroPython 开发：

- 在您的开发板上烧录合适的固件
- 在 PC 机上安装 RT-Thread MicroPython 开发环境并连接上开发板

接下来就可以尽情挥洒您的创意了，更详细的内容可以点击下文中的链接来进一步了解。

### 下载合适的固件

- [Sparrow One Board firmware](https://www.rt-thread.org/qa/forum.php?mod=viewthread&tid=12305&extra=page%3D1%26filter%3Dtypeid%26typeid%3D20)

### 安装 IDE 开发环境

- [RT-Thread MicroPython develop environment](https://marketplace.visualstudio.com/items?itemName=RT-Thread.rt-thread-micropython)

## 开发资料

### 示例程序

以下示例程序可以在 RT-Thread MicroPython IDE 开发环境中直接添加到工程：

![sparrow_example](assets/sparrow_example.png)

### MicroPython 模块详解

- [MicroPython Librarys](https://github.com/RT-Thread-packages/micropython/blob/master/docs/micropython-librarys.md)


## 联系我们

如果在使用的过程中遇到问题，您可以用如下方式联系我们：

- 在 github 上提交 issue
- [RT-Thread MicroPython 官方论坛](https://www.rt-thread.org/qa/forum.php?mod=forumdisplay&fid=2&filter=typeid&typeid=20)

- RT-Thread MicroPython 交流 QQ 群：703840633
