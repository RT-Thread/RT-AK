# RT-AK 开发手册

## 普通项目开发

> 在以支持的目标平台上进行开发

1. 了解本项目, 了解已支持的目标平台
3. 克隆本项目主仓库，
4. 使用 `RT-AK Tools`，请参考 `tools` 使用 [文档](../rt_ai_tools/README.md)， 代码运行的过程中参数指定已支持平台
5. 在集成了 `AI` 模型的 `BSP` 上进行应用代码开发，具体请参考 [RT-AK Lib API 手册](../rt_ai_lib/readme.md)

## 第三方目标平台适配

`RT-AK` 适配第三方目标平台无需更改 `RT-AK` 主体代码，只需要完成插件适配即可，分别是：

- 与 RT-AK Tools 相关的库插件拓展
- 与 RT-AK Lib 相关的工具插件拓展

![使用第三方目标平台插件](./imgs/20210331173329.png)

<center><font size=2 color="gray">RT-AK 拓展第三方目标平台</font></center>

可参考 `rt_ai_tools/platforms/plugin_examples` 或者 [STM32 插件 Github](https://github.com/RT-Thread/RT-AK-plugin-stm32)

插件开发流程：

1. 了解本项目, 查看贡献手册, 了解开发规范

2. 在 `rt_ai_tools/platforms` 路径下，新建两个文件夹

   - 工具插件：`plugin_<platform>`
   - 库插件：`plugin_<platform>/backend_plugin_<platform>`

   - 若要将插件项目上传到 `Github`，请及时更新 `platforms/support_platforms.json` 中的新增插件和插件仓库地址

3. 查看 [RT-AK Tools](https://github.com/RT-Thread/RT-AK/blob/main/RT-AK/rt_ai_tools/Documents/RT-AK%20Tools%20%E5%BC%80%E5%8F%91%E6%89%8B%E5%86%8C.md) 和 [RT-AK Lib](https://github.com/RT-Thread/RT-AK/blob/main/RT-AK/rt_ai_lib/readme.md) 开发手册，内含插件开发，阅读部分源码

4. 设计 `RT-AK` 与目标平台的工具插件和库插件

5. 配备相应文档系统测试

6. 将插件项目代码上传到 `Github`，在 `RT-AK/Document/support_platforms.json` 下新增插件和插件仓库地址

7. 管理员审核

---

几乎所有项目代码可以修改提交，注意以下一点：

- 主项目除了 `rt_ai_tools/platforms` 和 `platforms/support_platforms.json`，其他部分不应包含任何第三方目标平台相关代码

## RT-AK Lib 手册

在嵌入式端使用 `AI` 模型推理时的 `API` 手册以及插件开发文档：[RT-AK Lib API 手册](../rt_ai_lib/readme.md)

## RT-AK Tools 手册

`RT-AK Tools` 运行时 `API` 使用手册以及开发与适配文档请参考: [RT-AK Tools 开发手册](../rt_ai_tools/Documents/RT-AK Tools 开发手册.md)

