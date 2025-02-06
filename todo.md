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

## 2025/2/6: 配置系统04_复杂类型的支持
- [x] FromStr/ToStr，支持vector类型的配置