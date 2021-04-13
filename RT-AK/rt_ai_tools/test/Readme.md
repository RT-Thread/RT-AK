> python 单元测试；
>
> 使用的是 pytest 这个库

先执行：

```shell
$ pip3 install -r requirements.txt
```



# 1. 测试等级参考

1. Level1：正常流程可用，即一个函数在输入正确的参数时，会有正确的输出
2. Level2：异常流程可抛出逻辑异常，即输入参数有误时，不能抛出系统异常，而是用自己定义的逻辑异常通知上层调用代码其错误之处
3. Level3：极端情况和边界数据可用，对输入参数的边界情况也要单独测试，确保输出是正确有效的
4. Level4：所有分支、循环的逻辑走通，不能有任何流程是测试不到的
5. Level5：输出数据的所有字段验证，对有复杂数据结构的输出，确保每个字段都是正确的



作者：gashero
链接：[各位都是怎么进行单元测试的？ - gashero的回答](https://www.zhihu.com/question/27313846/answer/120164282)

# 2. 测试用例

### 2.1 验证是否列出模型支持范围

- 测试对象: `rt_ai_tools/README.md`

- 测试数据: `feature/u1s1_3.feature`

  | File      | platforms         |
  | --------- | ----------------- |
  | README.md | -                 |
  | READ.md   | -                 |
  | README.md | stm32 k210 tflite |

- 测试函数: `step_defs/test_u1s1_3.py`

