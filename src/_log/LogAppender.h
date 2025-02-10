#pragma once 

#include <memory>
#include <string>
#include "Logger.h"
#include "LogLevel.h"
#include "LogFormatter.h"
#include <fstream>
#include "src/mutex/mutex.h"

namespace shs {
// 日志输出地
class LogAppender {
public:
    using ptr = std::shared_ptr<LogAppender>;
    // using MutexType = Mutex;
    using MutexType = NullMutex;

    LogAppender(LogLevel level, LogFormatter::ptr formatter);
    LogAppender() {}
    virtual ~LogAppender() {}                                   // 虚析构函数，使得在使用基类的指针析构时也会先调用派生类的析构函数

    virtual void log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) = 0;  // 纯虚函数，子类必须实现

    // 对输出格式的操作
    void setFormatter(LogFormatter::ptr formatter) { 
        MutexType::Lock lock(m_mutex);
        m_formatter = formatter; 
    }
    LogFormatter::ptr getFormatter() const { 
        MutexType::Lock lock(m_mutex);
        return m_formatter; 
    }

    // 对日志级别的操作
    void setLevel(LogLevel level) { 
        MutexType::Lock lock(m_mutex);
        m_level = level; 
    }
    LogLevel getLevel() const { 
        MutexType::Lock lock(m_mutex);
        return m_level; 
    }

    virtual YAML::Node toYamlNode() = 0;
    virtual std::string toYamlString() = 0;
protected:                                                      // 保护模式：public继承的子类还可以直接访问
    LogLevel m_level = LogLevel::DEBUG;                                           // 日志级别
    LogFormatter::ptr m_formatter = std::make_shared<LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");   // 输出格式
    mutable MutexType m_mutex;
};


// 输出到控制台的 Appender
class StdoutLogAppender: public LogAppender {
public:
    using ptr = std::shared_ptr<StdoutLogAppender>;
    
    void log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) override;

    YAML::Node toYamlNode();
    std::string toYamlString();
private:
};

// 输出到文件的 Appender
class FileLogAppender: public LogAppender {
public:
    using ptr = std::shared_ptr<FileLogAppender>;

    FileLogAppender(const std::string& filename);

    void log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) override;

    // 重新打开文件，文件打开成功返回 true
    bool reopen();
    
    YAML::Node toYamlNode();
    std::string toYamlString();
private:
    std::string m_filename;     // 文件名
    std::ofstream m_filestream; // 文件输出流
};
}

