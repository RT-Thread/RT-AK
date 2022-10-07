## **rtthread** – 系统相关函数

**rtthread** 模块提供了与 RT-Thread 操作系统相关的功能，如查看栈使用情况等。

### 函数

#### rtthread.current_tid()  
返回当前线程的 id 。

#### rtthread.is_preempt_thread()  
返回是否是可抢占线程。

### 示例

```
>>> import rtthread
>>> 
>>> rtthread.is_preempt_thread()       # determine if it's a preemptible thread
True
>>> rtthread.current_tid()             # current thread id
268464956
>>> 
```
