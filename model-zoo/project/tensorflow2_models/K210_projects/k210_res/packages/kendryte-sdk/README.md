# 勘智SDK驱动包

## 1、介绍

勘智SDK驱动包是一份RT-Thread上勘智K210处理器对应的外设驱动包，主要是对K210上提供的standalone sdk提
供一份RT-Thread移植、包装，能够融合到RT-Thread系统中。

### 1.1 目录结构

| 名称 | 说明 |
| ---- | ---- |
| kendryte-standalone-sdk  | 勘智K210处理器的外设驱动包 |

### 1.2 许可证

许可证协议同kendryte-standalone-sdk的许可证

### 1.3 依赖

- RT-Thread 4.0+
- K210 BSP

## 2、如何使用

正常的情况下，当使用K210 BSP时，这份外设驱动包就会自动使能。而驱动包本身在menuconfig中位于如下路径：

```
RT-Thread online packages
    peripheral libraries and drivers --->
        [*] the kendryte-sdk package for rt-thread
```

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。
