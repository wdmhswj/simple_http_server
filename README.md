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
自定义类型，需要实现 shs::Lexical_cast,偏特化，实现后就可以支持 Config 解析自定义类型，自定义类型可以和常规stl容器一起使用

配置的事件机制：
- 当一个配置项发生修改的时候，可以反向通知对应的代码，回调

## 日志系统整合配置系统
```yaml
logs:
    - name: root
      level: (debug, info, warn, error, fatal)
      formatter: '%d%T%p%T%t%m%n'
      appender:
        - type: (StdoutLogAppender, FileLogAppender)
          level: (debug, ...)
          file: /logs/xxx.log
```
```cpp
    shs::Logger g_logger = shs::LoggerMgr::GetInstance()->getLogger(name);
    SHS_LOG_INFO(g_logger) << "xxxx log";
```
```cpp
static Logger::ptr g_log = SHS_LOG_NAME("system");
//m_root, m_system-> m_root, 当logger的appenders为空，使用root写logger
```
yaml 配置文件修改日志系统的 loggers 是通过在 log.cpp 源文件中创建一个全局变量`ConfigVar<std::set<LogDefine>>::ptr g_log_defines = Config::Lookup(std::set<LogDefine>(), "logs", "logs config");`，并在 LogIniter 结构体构造函数中为此全局变量添加回调函数，用于监听此配置变量值的变化，从而借助 SHS_LOG_NAME() 宏修改日志系统中对应的 logger

## 线程模块
暂时使用 std::thread (为了跨平台)


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
- CMake库文件安装：`cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr/local`, `cmake --build . --target install`
- 在全局构造函数中使用 Lookup 可能导致静态初始化顺序问题。需要使用 Meyer's Singleton 模式来解决：
```cpp
class Config {
public:
    using ConfigVarMap = std::map<std::string, ConfigVarBase::ptr>;
    
    static ConfigVarMap& GetDatas() {
        static ConfigVarMap s_datas;
        return s_datas;
    }
    
    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const T& default_value, 
                                           const std::string& name,
                                           const std::string& description="") 
    {
        auto& m_datas = GetDatas();  // 使用局部静态变量
        auto it = m_datas.find(name);
        if(it != m_datas.end()) {
            // ... 剩余代码不变
        }
        auto ptr = std::make_shared<ConfigVar<T>>(default_value, name, description);
        m_datas.emplace(name, ptr);
        return ptr;
    }
};
```
- 全局对象的构造和初始化在main函数之前
- C++11之后可以通过将构造函数声明为 delete 来实现禁用，不管作用域是public还是private
- thread_local 关键词只对声明于命名空间作用域的对象、声明于块作用域的对象及静态数据成员允许。它指示对象拥有线程存储期。它能与 static 或 extern 结合，以分别指定内部或外部链接（除了静态数据成员始终拥有外部链接），但附加的 static 不影响存储期。 线程存储期: 对象的存储在线程开始时分配，而在线程结束时解分配。每个线程拥有其自身的对象实例。唯有声明为 thread_local 的对象拥有此存储期。 thread_local 能与 static 或 extern 一同出现，以调整链接。
  - thread_local描述的对象在thread开始时分配，而在thread结束时分解。
  - 一般在声明时赋值，在本thread中只执行一次。thread_local 
  - 描述的对象依然只在作用域内有效。
  - 描述类成员变量时，必须是static的。


## todo
- [x] `"%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"` 日志格式解析失败（`str = m_pattern.substr(i+1, n-i-1);`中`n-i-1`错写为`n-i-i`）
- [ ] 减少日志模块的耦合度
    - 删去 Logger 类中的格式对象 m_formatter
- [ ] config 和 log 模块相互包含头文件，需要考虑如何解耦