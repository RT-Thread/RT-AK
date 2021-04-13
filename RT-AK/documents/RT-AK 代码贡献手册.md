# RT-AK 代码贡献手册

非常感谢您对 `RT-AK` 开源 `AI` 部署框架的支持, 因为您的支持我们将会做的越来越好.

## 1. RT-AK 主项目代码部分

确保您需要提交的内容符合 google 的 python 开发编程风格，并且能够顺利通过 CI 检测即可

## 2. 拓展第三方目标平台插件

您将需要提交的内容：

一个独立的插件仓库，将插件仓库地址新增至：`platforms/support_platforms.json`

插件仓库里面包含：

1. `RT-AK Lib`

   用于对接 `RT-AK` 与目标平台的后端适配层设计与实现代码

    * 命名：`backend_xxx`
    * 位于：`rt_ai_tools/platforms/<platform>`
    * 示例：`rt_ai_tools/platforms/example/backend_example`

2. `RT-AK Tools`

	使用 `python` 脚本实现模型转换和加载 `RT-AK Lib` 到项目工程中

    - 命名：`<platform>`
    - 位于：`rt_ai_tools/platforms`
    - 示例：`rt_ai_tools/platforms/example`

3. 测试代码

4. 文档说明

详细的开发文档请移步各自的开发文档介绍。

## 3. 准备阶段

* 克隆最新的 `RT-AK` 源码

## 4. 开发阶段

1. 按照例程需求，编写例程代码，代码编写需符合《[RT-Thread 编程风格](https://github.com/RT-Thread-Studio/sdk-bsp-stm32h750-realthread-artpi/blob/master/documents/coding_style_cn.md)》，Git 提交日志信息描述清晰
2. 软件代码尽可能的简洁，保证用户看到的代码清晰，容易理解。在 `menuconfig` 中也不要开启无关选项

3. 项目工程 `README`

- 创建的例程必须详细写明该例程实现的功能
- 写明该例程的操作说明
- 写明该例程的注意事项
- 填写维护人联系信息

## 5. 审核阶段

- 该阶段主要指例程开发完成，提交代码至项目仓库，交给管理员 `review` 的阶段
- 推送代码至远端分支，在 `CI` 构建通过后，发起 `merge request` 并指派给管理员
- 等待 `review` 的结果，收到改动要求后，重新更正，直到管理员合并代码。