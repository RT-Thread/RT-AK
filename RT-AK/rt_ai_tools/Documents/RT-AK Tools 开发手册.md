<center><h1>RT-AK Tools 开发手册</h1></center>

- [简介](#简介)
- [如何下载 RT-AK Tools](#如何下载-RT-AK-Tools)
- [RT-AK Tools 架构](#RT-AK-Tools-架构)
  - [1. 开发背景](#1.-开发背景)
  - [2. 功能实现](#2.-功能实现)
  - [3. 拓展插件代码贡献](#3.-拓展插件代码贡献)
- [拓展第三方目标平台插件](#拓展第三方目标平台插件)
- [RT-AK Tools 代码贡献标准](#RT-AK-Tools-代码贡献标准)
  - [审核阶段](#审核阶段)

## 简介

本文将详细介绍 `RT-AK Tools` 开发流程，让用户掌握如何使用 `RT-AK Tools` 进行应用开发，包括但不局限于以下内容：

- 如何下载 `RT-AK Tools`
-  `RT-AK Tools` 架构
- 拓展第三方目标平台插件
- `RT-AK Tools` 代码贡献标准

## 如何下载 RT-AK Tools

当你在克隆主项目的时候，`RT-AK Tools` 会随之克隆到本地。

相关插件仓库地址在 `platforms/support_platforms.json` 文件中；

相关插件代码在 `./RTAK/rt_ai_tools/platforms` 路径下。

该路径存放的就是第三方目标平台插件和示例插件模板。

其中，`plugin_examples` 是示例插件模板。

`k210` 和 `stm32` 分别是 `RT-AK Tools` 所拓展的 `k210 kpu` 和 `stm32`  支持插件。

- **k210 尚未开源**

##　RT-AK Tools 架构

![待更新](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210402101940.png)

*<font color="red">架构图待更新</font>*

*其中红色字体部分是待开发功能*

重要的两个组成部分:

1. 模型转成 `RT-AK` 所支持的格式文件
2. 复制 `RT_AK Lib` 到 `BSP` 项目工程中, 如果有静态库依赖, 一并复制

## 拓展第三方目标平台插件

### 1. 开发背景

- Python 开发环境, 版本 >= 3.7
- 存在第三方目标平台推理库, 比如 `STM32` 的 `X-CUBE-AI`

### 2. 功能实现

以下功能均用 `python` 脚本实现

- [ ] 插件模型转换, 比如 `STM32` 将 `Keras` 模型 转成 `c-model`
- [ ] 将转换后的模型复制到  `BSP` 项目工程中
- [ ] 如果存在第三方静态库依赖, 同样复制到  `BSP` 项目工程中
- [ ] 生成 `rt_ai_<model_name>_model.h` 文件, 包含模型的相关信息, 可参考 [RT-AK 快速上手.md](../../documents/RT-AK 快速上手.md)
- [ ] 生成 `rt_ai_<model_name>_model.c` 文件, 将模型的相关信息注册到 `RT-AK Lib` 后端, 可参考 [RT-AK 快速上手.md](../../documents/RT-AK 快速上手.md)

---

注意在 `rt_ai_tools/platforms/<platform>`  文件夹下有一个 `backend_<platform>` 文件夹，里面存放的对接目标平台和 `RT-AK Lib` 的适配代码。详细开发规范请阅读 `RT-AK Lib` 中的 `README` 文档。

### 3. 拓展插件代码贡献

| 推理库插件            | 格式                                                         | 备注                                                         |
| --------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 拓展代码位于          | 在 `rt_ai_tools/platforms` 路径下新建一个 `plugin_<platform>` 的 `git` 空仓 | RT-AK Tools 和 Lib 开发代码应位于该 `plugin_<platform>` 目录下 |
| 命名                  | 统一命名为 `plugin_<platform>`，例如 `plugin_example`        |                                                              |
| 插件所需参数          | 在`platforms/<platform>` 下创建 `<platform>_parser.py`       | 其中, `--enable_rt_lib` 参数必须指定, 命名: `RT_AI_<platform>` |
| 插件运行代码          | 1. 在`rt_ai_tools/platforms/<platform>` 下创建 `<platform>.py` <br>2. 在 `<platform>.py` 文件中创建 `Pluging` 类<br>3. 在 `Pluging` 类中创建 `run_plugin` 函数 |                                                              |
| 插件完成开发准备提 PR | 在主项目中 `RT-AK/Document/support_platforms.json` 下新增插件和插件仓库地址 | 管理员审核                                                   |

- 尽量避免 `print`, 请使用 `logging`
- 可参考 `rt_ai_tools/platforms/plugin_examples`

## RT-AK Tools 代码贡献标准

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

- [ ] **TO DO LIST:** CI 更新