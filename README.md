# Simple Http Server
## 资料
- https://github.com/sylar-yin/sylar
- https://www.bilibili.com/video/BV184411s7qF
## 日志模块
### 架构
```
Logger(定义日志类别)
Formatter(定义日志格式)
Appender(日志输出的地方)
```

## 知识点
- std::enable_shared_from_this
- 宏定义 与 inline
- std::shared_ptr 的 reset 方法：重置智能指针的状态
- C++ 日期时间相关：
    - https://www.runoob.com/cplusplus/cpp-libs-ctime.html
    - localtime_r 是 C++ 中用于将时间转换为本地时间的线程安全函数。它是 localtime 函数的线程安全版本，适用于多线程环境。localtime_r 在 Linux 平台下使用，而在 Windows 平台下，使用的是 localtime_s。
## todo
- "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n" 日志格式解析失败