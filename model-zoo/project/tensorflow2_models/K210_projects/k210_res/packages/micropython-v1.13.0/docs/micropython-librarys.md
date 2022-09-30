# MicroPython 库

### MicroPython 标准库

- [`Builtin functions and exceptions`](std-librarys/builtins.md) – 内置函数与异常
- [`cmath`](std-librarys/cmath.md)  – 复数运算函数功能
- [`gc`](std-librarys/gc.md) – 控制垃圾收集器
- [`math`](std-librarys/math.md) – 数学函数功能
- [`sys`](std-librarys/sys.md) – 系统特定功能
- [`uarray`](std-librarys/uarray.md) – 数组存储功能
- [`ubinascii`](std-librarys/ubinascii.md) – 二进制与 ASCII 码转换功能
- [`ucollections`](std-librarys/ucollections.md) – 集合与容器类型
- [`uerrno`](std-librarys/uerrno.md) – 系统错误码
- [`uhashlib`](std-librarys/uhashlib.md) – 哈希算法
- [`uheapq`](std-librarys/uheapq.md) – 堆队列算法
- [`uio`](std-librarys/uio.md) – 输入输出流
- [`ujson`](std-librarys/ujson.md) – JSON 编解码
- [`uos`](std-librarys/uos.md) – 基本的操作系统服务
- [`ure`](std-librarys/ure.md) – 正则表达式
- [`uselect`](std-librarys/uselect.md) – 在一组 streams 上等待事件
- [`usocket`](std-librarys/usocket.md) – socket 模块
- [`ussl`](std-librarys/ussl.md) – SSL/TLS 模块
- [`ustruct`](std-librarys/ustruct.md) – 原生数据类型的打包和解包
- [`utime`](std-librarys/utime.md) – 时间相关功能
- [`uzlib`](std-librarys/uzlib.md) – zlib 解压
- [`_thread`](std-librarys/_thread.md) – 多线程支持

### MicroPython 特定库

在 RT-Thread 移植的 MicroPython 版本中，实现了如下特定功能库：

- [`micropython`](spec-librarys/micropython.md) – 实现 MicroPython 内部功能访问与控制
- [`rtthread`](spec-librarys/rtthread.md) – RT-Thread 系统功能模块
- [`machine`](spec-librarys/machine.md) – 硬件控制模块
    - [Pin](spec-librarys/machine/Pin.md) 
    - [I2C ](spec-librarys/machine/I2C.md)
    - [SPI](spec-librarys/machine/SPI.md)
    - [UART](spec-librarys/machine/UART.md)
    - [LCD](spec-librarys/machine/LCD.md)
    - [RTC](spec-librarys/machine/RTC.md)
    - [PWM](spec-librarys/machine/PWM.md)
    - [ADC](spec-librarys/machine/ADC.md)
    - [WDT](spec-librarys/machine/WDT.md)
    - [TIMER](spec-librarys/machine/Timer.md)

- [`network`](spec-librarys/network.md) – 网络功能配置模块
    - [wlan](spec-librarys/network/wlan.md) 
