Other Language: [Chinese](README.md)

### Table of Contents

- [1. RT-AK](#intro)
- [2. CI Status](#ci-status)
  - [Official](#official)
  - [Community](#community)
- [3. Help](#help)
- [4. Developer Documents](#developer-documents)

# Intro

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](./LICENSE) [![](https://img.shields.io/badge/Plugin-STM32-brightgreen)](https://github.com/RT-Thread/RT-AK-plugin-stm32) [![](https://img.shields.io/badge/Plugin-K210-brightgreen)](https://github.com/RT-Thread/RT-AK-plugin-k210)

**`RT-AK`: `RT-Thread AI Kit`**

`RT-AK` is AI toolkit for RT-Thread RTOS, which makes it possible to deploy the AI model into a RT-Thread project, by just one command.

`RT-AK` not only provides `API` to support application development, but also takes advantage of the target platform facility for better performance.

With `RT-AK`, it requires just one command to deploy an `AI` model, like this:

```bash
$ python aitools.py --model xxx...
```

For more details, please refer [./RT-AK/README.md](./RT-AK/README.md) and [Quick Start](./RT-AK/documents/RT-AK%20Quick%20Start.md).

---

**We focus on make it easy to land AI on the edge.**

# CI Status

## Official

`RT-AK` Tools Support PC OS:

| Build Type   | Status                                                       | Artifacts |
| ------------ | ------------------------------------------------------------ | --------- |
| Windows 10   | ![](https://img.shields.io/badge/RT--AK-passing-blackgreen.svg) |           |
| Ubuntu 18.04 | ![](https://img.shields.io/badge/RT--AK-passing-blackgreen.svg) |           |
| Mac          | -                                                            |           |

`RT-AK` Plugins For Target Platform:

| Build Type | Status                                                       | Artifacts                                                    |
| ---------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| STM32      | [![](https://img.shields.io/badge/Plugin-STM32-brightgreen)](https://github.com/RT-Thread/RT-AK-plugin-stm32) | [RT-AK STM32 Quick Start](./RT-AK/documents/RT-AK%20STM32%20Quick%20Start.md) |
| K210       | [![](https://img.shields.io/badge/Plugin-K210-brightgreen)](https://github.com/RT-Thread/RT-AK-plugin-k210) | [RT-AK K210 Quick Start](https://github.com/RT-Thread/RT-AK-plugin-k210/blob/main/docs/RT-AK%20K210%20Quick%20Start.md) |
| -          | -                                                            | -                                                            |

Both `STM32` and `K210` original toolchain come with model optimization support, but so far only `K210` plugin enabled qunatilization. `STM32` plugin doesn't enable it, yet.


Notes:

- For `STM32` platform, please qunatilize the model before run RT-AK, or it will generate codes using float.
- For `K210`, the neural model must be raw model (not quantilized), RT-AK plugin will take care internally per your specification.

For more details, please refer documents in main project (RT-AK) and corresponding platform plugin.

## Community

| Build Type | Status | Artifacts |
| ---------- | ------ | --------- |
|            |        |           |

# Help

- [RT-AK FAQ](./RT-AK/documents/RT-AK FAQ.md)

- [RT-Thread Forums](https://club.rt-thread.io/)

  <img src="./RT-AK/documents/imgs/20210114105417.png" style="zoom:50%;" />
  
- Business: please contact business@rt-thread.com for commercial support, like develop plugin for proprietary hardware or consultant service.

# Developer Documents

For more internals, please read documents in [RT-AK/documents](RT-AK/documents), welcome to contribute document, demo project and platform plugin. PS, please use stand alone repository for the plugin.

- [RT-AK FAQ.md](RT-AK/documents/RT-AK FAQ.md)
- [RT-AK Contributor Guide.md](RT-AK/documents/RT-AK Contributor Guide.md)
- [RT-AK Dev Guide.md](RT-AK/documents/RT-AK Dev Guide.md)
- [RT-AK Quick Start.md]((RT-AK/documents/RT-AK%20Quick%20Start.md)
- [RT-AK Notes on AI Model.md](RT-AK/documents/RT-AK Notes on AI Model.md)

- [RT-Thread Prgramming Guide.pdf (Chinese)](RT-AK/documents/RT-Thread编程指南.pdf)