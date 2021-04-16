# RT-AK 开发手册

## 普通项目开发

> 仅使用

1. 了解本项目, 了解已支持的目标平台
2. 选择一个已支持平台
3. 克隆本项目主仓库，
4. 使用 `RT-AK Tools`, 参考 `tools` 使用[文档](../rt_ai_tools/README.md)
5. 在集成了 `AI` 模型的 `BSP` 上进行开发，具体请参考[ RT-AK Lib API 手册](../rt_ai_lib/readme.md)

## 插件拓展开发

> 适配第三方目标平台插件拓展，要完善两部分， RT-AK Tools 和 Lib 组件的代码
>
> 可参考 `rt_ai_tools/platforms/plugin_examples`

1. 在 `rt_ai_tools/platforms` 路径下，新建一个 `plugin_<platform>` 的 `git` 空仓，`tools` 和 `lib` 拓展代码应位于 `rt_ai_tools/platforms/plugin_<platform>` 目录下
2. 了解本项目, 查看贡献手册, 了解开发规范
3.  查看 `tools` 和 `lib` 参考手册, 阅读部分源码
4. 设计和实现目标平台 `tools` 插件与适配层
5. 配备相应系统测试
6. ~~提交代码~~，在 `RT-AK/Document/support_platforms.json` 下新增插件和插件仓库地址
7. 管理员审核

---

几乎所有项目代码可以修改提交，注意以下一点：

- 主项目除了 `rt_ai_tools/platforms` 和 `platforms/support_platforms.json`，其他部分不应包含任何第三方目标平台相关代码

## RT-AK Lib 手册

`RT-AK` 运行时库 API 使用手册以及开发与适配文档请参考 [rt_ai_lib/readme](../rt_ai_lib/readme.md)

## RT-AK Tools 手册

`RT-AK` 运行时库 `API` 使用手册以及开发与适配文档请参考 [RT-AK Tools 开发手册](../rt_ai_tools/Documents/RT-AK Tools 开发手册.md)

