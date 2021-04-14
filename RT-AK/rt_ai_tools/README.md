- [0x01 准备工作](#0x01-准备工作)
- [0x02 目录结构](#0x02-目录结构)
- [0x03 参数说明](#0x03-参数说明)
- [0x04 演示示例](#0x04-演示示例)
- [0x05 流程&功能](#0x05-流程&功能)

# RT-AK Tools

> Update: 
>
> 新增查看模型信息模块，目前仅支持 `keras`，需要 `tensorflow>=2.2`，
>
> 运行结束之后将会在该目录下生成一个 `<model_name>_info.txt` 文件，示例：[keras_mnist_info.txt](./utils/keras_mnist_info.txt)
>
> ```shell
> $ cd utils
> 
> # 默认是 rt_ai_tools/Model/keras_mnist.h5
> $ python model_info.py --model=<your_model_path>
> ```
>

- What is RT-AK Tools?

- How to use RT-AK Tools? The key file is aitools.py

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210331183423.png)

# 0x01 准备工作

- Windows 10
- Python >= 3.7
- BSP
- AI Model

# 0x02 目录结构

```bash
D:\Project\edge-ai\RT-AK\rt_ai_tools>tree /a
卷 软件 的文件夹 PATH 列表
卷序列号为 E67E-D1CA
D:.
+---Documents
+---Model
+---platforms
|   +---example
|   +---k210  # 未开源，模型支持: TFLite | Caffe | ONNX
|   \---stm32  # 模型支持: Keras | TFLite | Caffe | ONNX
+---test
|   +---data
|   +---features
|   \---step_defs
\---utils
```

| Dir         | Description                                            |
| ----------- | ------------------------------------------------------ |
| `Documents` | `RT-AK Tools` 相关文档                                 |
| `Model`     | 神经网络模型库                                         |
| `platforms` | 第三方目标平台插件，比如：`STM32`， `K210`             |
| `test`      | `RT-AK Tools` 测试用例，使用的是 `Pytest-bdd` 测试框架 |

# 0x03 参数说明

| Parameter             | Description                                                  |
| --------------------- | ------------------------------------------------------------ |
| `--log`               | `log` 日志存放路径，默认是空，如果有，则将保存log日志文件，例如：`./log.log` |
| `--project`           | `OS+BSP` 项目工程文件夹，默认为空，**需要用户指定**          |
| `--model`             | 神经网络模型文件路径，默认为 `./Models/keras_mnist.h5`       |
| `--model_name`        | 神经网络模型转换后新的模型名，默认是 `network`               |
| `--rt_ai_lib`         | `RT-Thread` 官方提供的 `RT-AK Lib` ，默认是 `../rt_ai_lib`   |
| `--support_platforms` | 存放 `RT-AK` 支持的第三方目标平台插件，`platform: platform_git_url` 格式，默认是`platforms/support_platforms.json` |
| `--platform`          | 指定目标平台信息，目前支持：`stm32`、`k210`，默认是 `example`，具体体可用的目标平台由 `platforms/xxx.json` 注册 |

```shell
# 查看 ai_tools 的所有参数信息
$ python aitools.py --help
```

![image-20210409175546872](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210414173800.png)

# 0x04 演示示例

请查阅：[../README.md](../README.md) 中的第 3 章节 - 演示示例

# 0x05 流程&功能

## 5.1 RT-AI Tools 内部流程

1. 加载基础参数
2. 设置 `logging` 等级
3. 克隆/拉取最新第三方目标平台插件仓库
4. 加载插件参数
5. 执行插件相关工作
6. 打开 `RT-AK Lib` 中插件的的适配层
7. 复制 `RT-AK Lib` 到项目工程中

## 5.2  RT-AI Tools 内部功能函数

| Index | Function                    | Description                                                  |
| :---: | --------------------------- | ------------------------------------------------------------ |
|   1   | install_plugin.git_plugin() | 克隆插件仓库，若存在，则拉取最新的仓库代码                   |
|   2   | load_sub_parsers()          | 解析指定目标平台插件参数                                     |
|   3   | Plugin.run_plugin()         | 运行指定目标平台插件，比如模型转换。如果有相关库依赖，将相关库复制到 `project` 路径下 |
|   4   | enable_platform()           | 开启 `RT-AK Lib` 中目标平台相关库支持，如果已经开启过某个平台，先关闭后打开新的目标平台插件相关库 |
|   5   | load_rt_ai_lib()            | 复制 `RT-AK Lib` 到项目工程中，如果已经存在，则先删除后复制  |
