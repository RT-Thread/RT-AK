# Q&A

## Q1

转换后的模型信息, 比如输入输出数量, 每个输出占用字节数在哪里查看? 

**A:** 在tools生成的模型注册源码中, rt_ai_xxx_model.h中使用宏的方式展示出了模型运行时的一些资源信息.

## Q2

RT-AK框架能自定义输入输出的内存地址吗?

**A:** 部分平台可以, 查看文件代码文件backend_xxx.h, 使用`rt_ai_config()`函数进行命令配置.

## Q3

RT-AK可以查看运行时的一些状态或性能信息吗? 

**A:** 可以, 通过配置aiconfig.h源文件中的宏参数, 打开或关闭运行时状态打印. 暂只支持推理时间信息统计.

## Q4

执行过程中遇到 Git 报错

**A:** 将 Git 添加到系统环境变量即可

## Q5

编译成功，但是烧录失败，问题是在于下载算法没有设置，

- Keil 用户请参考：[Q7：使用MDK 下载程序失败](https://art-pi.gitee.io/website/docs/#/development/ART-Pi_常见问题解答?id=q7：使用mdk-下载程序失败)

## Q6

缺少 ST-Link 驱动，在设备管理器查看 COM 口的时候，发现是个黄色的感叹号

解决：[需要更新 ST-LINK 驱动](https://art-pi.gitee.io/website/docs/#/development/ART-Pi_常见问题解答?id=a-需要更新-st-link-驱动，a-hrefhttpswwwstcomzhdevelopment-toolsstsw-link009html-target_blank-relnoopener下载地址)   [下载地址](https://www.st.com/zh/development-tools/stsw-link009.html)

Q7：

如果在代码运行的过程中，发现 git 克隆插件仓库的时候，极为缓慢，

**A:** 可以将插件仓库复制到 gitee，同时更改 `RT-AK/rt_ai_tools/platforms/support_platforms.json` 地址即可实现 git 加速

