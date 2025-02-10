# todo list
## 2025/1/21
- [x] Logger::Logger
- [x] %p 用户名
- [x] 线程id 协程id
- [x] #define 宏
- [x] LogEventWrap 	
- [x] vasprintf
- [x] LoggerManager
- [x] Singleton 单例模式

## 2025/1/22: 配置系统，通过 YAML 文件解析配置
- [x] << 运算符重载（不需要，注意宏定义直接替换代码的特性）
- [x] class ConfigVarBase
- [x] boost 库安装配置使用
- [x] template class ConfigVar
- [x] LoggerManager::getRoot()
- [x] #define SYLAR_LOG_ROOT
- [x] test_config 测试

## 2025/2/5: 配置系统02_yaml
- [x] 理解配置系统到底是什么：将项目的一些配置写入yaml文件，通过yaml-cpp库读取配置文件并解析，日志系统可以辅助打印加载的配置信息
- [x] 学习yaml格式
- [x] 安装/使用C++库：yaml-cpp, `cmake -DCMAKE_INSTALL_PREFIX=D:\libs\yaml-cpp-0.8.0 -G "MinGW Makefiles" ..`，`cmake --build . --target install`
- [x] bin/conf/log.yml
- [x] test_config.cpp: test_yaml()

## 2025/2/5: 配置系统03_yaml整合
- [x] ConfigVarBase m_name 大小写
- [x] LoadFromYaml()
- [x] ConfigVarBase Config::LookupBase()
- [x] void test_config()
- [x] log.yaml

## 2025/2/6: 配置系统
- [x] FromStr/ToStr，支持vector类型的配置
- [x] 更多stl容器的支持: list/map/unordered_map/set/unordered_set
- [x] test_config: #define XX
- [x] 解决key相同value类型不同时不会报错的问题
- [x] 自定义类型解析: 直接class，map< str, Person>, vector<Person>
- [x] fromString() catch 报错内容添加

## 2025/2/7: 配置系统/日志系统
- [x] 配置更变事件：<functional>库，使用std::map实现对std::function的比较和删除（添加/删除/获取这3个函数），在 setValue() 函数中先比较值是否更变，再调用回调函数
- [x] 日志系统整合配置系统：SHS_LOG_NAME, LoggerManager::getLogger(), Logger::log(), Logger::Logger(), g_log_defines, struct LogIniter
- [x] 日志系统整合：Logger::setFormatter/getFormatter, is_error（解析时加is_error判断），LogIniter，直接偏特化 LogDefine，而不是 std::set<LogDefine>, log.yml，LogLevel::fromString(), LogDefine偏特化
- [x] toYamlString(), test_log()

## 2025/2/8: 配置/日志系统整合
- [x] 文件路径改为相对路径

## 2025/2/9: 线程模块
- [x] thread.h/cpp: class Thread, 拷贝/赋值构造函数 delete, static thread_local Thread*, pthread_detach(), pthread_join(), Thread::run()
- [x] sylay.h, test_thread.cpp
- [x] 不同 OS 下线程号对应

## 2025/2/10: 线程模块
- [x] 信号量与互斥量：读写分离，出线程构造函数之前保证线程已经启动 