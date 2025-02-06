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
## 配置系统模块
原则：约定优于配置
```cpp
template<class T, class FromStr, class ToStr>
class ConfigVar;

template<class F, class T>
Lexical_cast;

// 容器偏特化，目前支持：
//                  vector, list, set, map, unordered_map, set, unordered_set
// map/unordered_map 支持 key=std::string
// Config::Lookup(key), key 相同，类型不同的，不会报错，这个需要后面处理
```
## 知识点
- std::enable_shared_from_this
- 宏定义 与 inline
- std::shared_ptr 的 reset 方法：重置智能指针的状态
- C++ 日期时间相关：
    - https://www.runoob.com/cplusplus/cpp-libs-ctime.html
    - localtime_r 是 C++ 中用于将时间转换为本地时间的线程安全函数。它是 localtime 函数的线程安全版本，适用于多线程环境。localtime_r 在 Linux 平台下使用，而在 Windows 平台下，使用的是 localtime_s。
- 使用以下宏的目的，一方面是减少代码量，另一方面是因为宏的机制是直接替换，使得`__LINE__`和`__FILE__`两个宏能够正确显示代码行号和文件名；同时之所以 SHS_LOG_LEVEL 宏定义中要调用 getSS()，是因为在之后使用该宏时可以在后面直接使用`<< "message"`以添加日志信息。
```cpp
#define SHS_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        shs::LogEventWrap(shs::LogEvent::ptr(new shs::LogEvent(logger, level, __FILE__, __LINE__, 0, shs::GetThreadIdBySyscall(), shs::GetFiberId(), time(0), "shs::Thread::GetName()"))).getSS()

#define SHS_LOG_DEBUG(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::DEBUG)
#define SHS_LOG_INFO(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::INFO)
#define SHS_LOG_WARN(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::WARN)
#define SHS_LOG_ERROR(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::ERROR_)
#define SHS_LOG_FATAL(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::FATAL)
```
- VA_ARGS 宏定义，用于宏定义中的可变参数列表，它可以让宏接受任意数量的参数
- typeid 运算符：获取一个表达式的类型信息，头文件 <typeinfo>
- std::dynamic_pointer_cast<> 用于将基类的智能指针转化为子类的智能指针（std::shared_ptr）
- std::stringstream 对象内容的清空应使用 str("") 方法，而不是 clear() (clear只是重置流对象的状态，)
## todo
- [x] `"%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"` 日志格式解析失败（`str = m_pattern.substr(i+1, n-i-1);`中`n-i-1`错写为`n-i-i`）
- [ ] 减少日志模块的耦合度
    - 删去 Logger 类中的格式对象 m_formatter