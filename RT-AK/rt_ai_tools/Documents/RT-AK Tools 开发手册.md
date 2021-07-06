<center><h1>RT-AK Tools 开发手册</h1></center>

- [1. 简介](#1.-简介)
- [2. 如何下载 RT-AK Tools](#2.-如何下载-RT-AK-Tools)
- [3. RT-AK Tools 架构](#3.-RT-AK-Tools-架构)
- [4. 拓展第三方目标平台插件](#4.-拓展第三方目标平台插件)
  - [4.1 开发背景](#4.1-开发背景)
  - [4.2 功能实现](#4.2-功能实现)
  - [4.3 插件开发规范](#4.3-插件开发规范)
- [5. RT-AK Tools 代码贡献标准](#5.-RT-AK-Tools-代码贡献标准)
  - [审核阶段](#审核阶段)

## 1. 简介

本文将详细介绍 `RT-AK Tools` 开发流程，让用户掌握如何使用 `RT-AK Tools` 进行应用开发，包括但不局限于以下内容：

- 如何下载 `RT-AK Tools`
-  `RT-AK Tools` 架构
- 拓展第三方目标平台插件
- `RT-AK Tools` 代码贡献标准

## 2. 如何下载 RT-AK Tools

当你在克隆主项目的时候，`RT-AK Tools` 会随之克隆到本地。

与目标平台相关的**插件**代码：

- 相关插件仓库地址在 `platforms/support_platforms.json` 文件中
  - 该路径存放的就是第三方目标平台插件和示例插件模板。

- 相关插件代码在 `./RTAK/rt_ai_tools/platforms` 路径下
  - 最开始的时候，该路径下面仅有一个 `plugin_example` 示例插件代码模板
  - 当用户运行一次 RT-AK Tools 之后（指定目标平台），代码会自动从对应的插件仓库地址拉取最新的插件项目代码

## 3. RT-AK Tools 架构

![待更新](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210402101940.png)

*<font color="red">架构图待更新</font>*

*其中红色字体部分是待开发功能*

**重要的两个组成部分:**

- `aitools`（上图中的上部分红色框框）

  与目标平台无关，完成以下工作：

  1. 根据用户输入的参数选择对应的目标平台
  2. 从目标平台插件仓库拉取插件代码，拉取的代码位于 `./RTAK/rt_ai_tools/platforms` 路径下
  3. 复制 `RT_AK Lib` 到 `BSP` 项目工程中, 如果有静态库依赖, 一并复制

- 平台插件（上图中的下部分红色框框）

  与目标平台相关，完成以下工作：

  1. 对用户输入的参数进行解析，此时的参数包含基础参数和目标平台插件参数
  2. 自检
  3. 在 `<BSP>/rtconfig.h` 中使能目标平台的控制宏，比如：`#define RT_AI_USE_CUBE`
  4. 将 `AI` 模型转换为目标平台所支持的格式，且复制到 `BSP `中，目前使用的 AI 模型转换工具均为原厂所提供
  5. 生成 AI 模型信息和与目标平台信息相关的模型声明文件，分别是：
     - `rt_ai_<model_name>_model.h`
     - `rt_ai_<model_name>_model.c`

## 4. 拓展第三方目标平台插件

### 4.1 开发背景

- `Python >= 3.7`
- `Windows 10` 或者 `Ubuntu 18.04`
- 目标平台已有 `AI` 模型转换工具和推理库

### 4.2 功能需求

需要研发 `RT-AK Tools` 和 `RT-AK Lib` 对应的插件代码

**RT-AK Tools 对应的工具插件代码功能列表：**

- `AI` 模型转换成目标平台支持的模型格式

- 将转换成功的 `AI` 模型复制到  `BSP` 项目中
  - 如果目标平台存在第三方静态库依赖，同样复制到  `BSP` 项目工程中

- 生成 `rt_ai_<model_name>_model.h` 文件
  - AI 模型的相关信息
  - 保存到 `<BSP>/applications` 路径下

- 生成 `rt_ai_<model_name>_model.c` 文件,
  - 与目标平台信息相关的模型声明文件
  - 保存到 `<BSP>/applications` 路径下

**RT-AK Lib 对应的库插件：**

-  适配 `RT-AK Lib` 与目标平台的后端代码

---

可参考 STM32 插件: [Github](https://github.com/RT-Thread/RT-AK-plugin-stm32)

### 4.3 插件开发规范

| 插件                  | 格式                                                         | 备注                                                         |
| --------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 代码存放              | 在 `rt_ai_tools/platforms` 路径下新建一个 `plugin_<platform>` 的文件夹 | 工具插件开发代码应位于该 `plugin_<platform>` 目录下<br>库插件开发代码应位于该 `plugin_<platform>/backend_plugin_<platform>` 目录下 |
| 命名                  | 插件统一命名为 `plugin_<platform>`                           | 例如 `plugin_example`，`plugin_stm32` 等                     |
| 插件参数              | 在`platforms/<platform>` 下创建 `<platform>_parser.py`       | 其中, `--enable_rt_lib` 参数必须指定, 命名: `RT_AI_<platform>` |
| 插件运行代码开发      | 1. 在`rt_ai_tools/platforms/<platform>` 下创建 `plugin_<platform>.py` <br>2. 在 `plugin_<platform>.py` 文件中创建 `Pluging` 类<br>3. 在 `Pluging` 类中创建 `run_plugin` 函数 |                                                              |
| 编程语言              | Python                                                       |                                                              |
| 插件完成开发准备提 PR | 若要将插件项目上传到 `Github`，请及时更新 `platforms/support_platforms.json` | 管理员审核                                                   |

- 尽量避免 `print`, 请使用 `logging`
- 可参考 `rt_ai_tools/platforms/plugin_examples` 或者 STM32 插件 Github](https://github.com/RT-Thread/RT-AK-plugin-stm32)

## 5. RT-AK Tools 代码贡献标准

1. `Python` 代码规范请满足 [ Google Python风格规范](https://zh-google-styleguide.readthedocs.io/en/latest/google-python-styleguide/python_style_rules/)
2. 代码设计参考 `plugin_example` ，统一命名格式为 `plugin_<platform>` 
3. `Git` 提交日志信息描述清晰
4. 软件代码尽可能的简洁，保证用户看到的代码清晰，容易理解
5. 项目工程 `README`
   - 创建的例程必须详细写明该例程实现的功能
   - 写明该例程的操作说明
   - 写明该例程的注意事项
   - 填写维护人联系信息

### 审核阶段

- 该阶段主要指例程开发完成，提交代码至项目仓库，交给管理员 `review` 的阶段
- 推送代码至远端分支，在 `CI` 构建通过后，发起 `merge request` 并指派给管理员
- 等待 `review` 的结果，收到改动要求后，重新更正，直到管理员合并代码。

----

**TO DO LIST:** 

- [ ] CI 更新
- [ ] 量化支持
- [ ] 测试用例设计

