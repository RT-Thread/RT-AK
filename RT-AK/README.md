# 目录

- [1. 简介](#简介)
- [2. 目录结构](#目录结构)
- [3. 演示示例](#演示示例)
  - [step 1 准备](#step-1-准备)
  - [step 2 使用 RT-AK](#step-2-使用-RT-AK)
  - [step 3 编译](#step-3-编译)
  - [step 4 下载固件](#step-4-下载固件)
  - [其他 - 项目实战](#其他-使用第三方目标平台项目实战)
- [4. 未来将完善](#未来将完善)

# 简介

`RT-AK` 是 `RT-Thread` 团队对 `RT-Thread` 系统所开发的 `AI` 套件，一键将 `AI` 模型部署到 `RT-Thread` 系统中，让用户可以更简单方便的开发自己的应用程序。

## 1.1 RT-AK 工作方式

`RT-AK` 的工作基于包含 RT-Thread 系统的 `BSP`，最后的结果将 AI 模型集成进 `BSP` 中，用户可以在上面根据 `RT-AK`  提供的 `API` 进行项目开发。

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210412202832.png)

<center><font size=2 color="gray">RT-AK 工作方式</font></center>

## 1.2 RT-AK 两大组件

![使用第三方目标平台插件](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210331173329.png)

<center><font size=2 color="gray">使用第三方目标平台插件</font></center>

`RT-AK` 主要包含两大组件件：

- RT-AK Lib

  适配第三方目标平台插件与 `RT-Thread` 系统的 Lib 库

  > 详细请查阅 👉 [rt_ai_lib/readme.md](./rt_ai_lib/readme.md)

- RT-AK Tools

  使用第三方目标平台插件进行模型转换以及加载 `RT-AK Lib` 相关工作

  > 详细请查阅 👉 [rt_ai_tools/README.md](./rt_ai_tools/README.md)

## 1.3 RT-AK 架构

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210412203032.png)

<center><font size=2 color="gray">RT-AK 架构图</font></center>

上图左半部分是 `RT-AK Tools` 的简单的架构图，工作在上位机；

上图右半部分是 `RT-AK Libs` 的简单的架构图，工作在单片机。

# 目录结构

```shell
D:\Project\edge-ai\RT-AK>tree /a
卷 软件 的文件夹 PATH 列表
卷序列号为 E67E-D1CA
D:.
+---documents
+---examples
+---rt_ai_lib
+---rt_ai_tools
\---test
    +---features
    +---image
    \---step_defs
```

| 文件夹        | 说明                                                     |
| ------------- | -------------------------------------------------------- |
| `documents`   | `RT-AK` 相关文档，比如常见问题、快速上手等               |
| `rt_ai_lib`   | `RT-AK Lib`，适配第三方目标平台插件与 RT-Thread 系统相关 |
| `rt_ai_tools` | `RT-AK Tools`，使用第三方目标平台插件进行模型转换等工作  |
| `test`        | `RT-AK` 测试用例，使用的是 `Pytest-bdd` 测试框架         |

# 演示示例

- [x] **仅支持 `Windows 10`**
- [x] **Python >= 3.7**
- [x] **该演示示例不使用任何第三方目标平台插件**

## step 1 准备

需要准备以下三份材料：

| Index |                           Prepare                            |
| :---: | :----------------------------------------------------------: |
|   1   |            硬件以及 `BSP`，示例使用的是 `ART-PI`             |
|   2   | 神经网络模型，示例使用的是 `./rt_ai_tools/Model/keras_mnist.h5` |
|   3   |                     将项目代码克隆到本地                     |

硬件选择 `ART-PI`，准备一个项目工程

- 可通过 `RT-Thread Studio` 来生成
- 或者直接下载我们所提供的 `BSP` 👉 [传送门](http://117.143.63.254:9012/www/RT-AK/sdk-bsp-artpi.zip)

> - [ ] **TO DO LIST**: 未来将会提供 `QEMU BSP`，无需依赖硬件

## step 2 使用 RT-AK

使用 RT-AK 之后，就可获得一个集成了 AI 的 BSP，就可以在上面愉快的开发啦！

```shell
$ cd rt_ai_tools

# 使用默认模型和默认目标平台，默认目标平台为 `./rt_ai_tools/platforms/example`
$ python aitools.py --project=<your_bsp_path>

# 指定模型路径和指定目标平台
$ python aitools.py --project=<your_bsp_path> --model=<model_path> --platform=<platform>
```

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210401101235.png)

<center><font size=2 color="gray">使用默认模型和默认目标平台工作</font></center>

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210401100920.png)

<center><font size=2 color="gray">指定模型参数和目标平台插件</font></center>

## step 3 编译

进入到你的项目工程路径，然后编译。

示例中选择的是 `scons` 编译 (在这里默认你已经配置好了 `RT-Thread` 的 [env](https://www.rt-thread.org/document/site/#) 环境，并且会使用 `scons -j 6` 编译)

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210331171829.png)

<center><font size=2 color="gray">scons 编译成功界面</font></center>

## step 4 下载固件

- 硬件：`ART-PI`
- 烧录工具：
  - `STM32Programmer`
  - `RT-Thread Studio`
  - `Keil`
  - ...

> 在后续版本中，演示示例将会基于 QEMU，该步骤将会进一步做简化
>
> 对第四步操作有困惑的同学请看👇项目实战部分，有完完整整的步骤和说明

## 其他-使用第三方目标平台项目实战

| 配置           | 说明                                           |
| -------------- | ---------------------------------------------- |
| 第三方目标平台 | `STM32`                                        |
| 项目工程       | `ART-PI BSP`                                   |
| 神经网络模型   | `rt_ai_tools/Model/keras_mnist.h5`             |
| 项目实战手册   | [RT-AK快速上手.md](documents/RT-AK快速上手.md) |

# 未来将完善

- [ ] Windows 7 支持
- [ ] Linux 支持
- [ ] Qemu 示例 DEMO
- [ ] ....

