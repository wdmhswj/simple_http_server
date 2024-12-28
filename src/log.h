#ifndef __SHS_LOG_H__
#define __SHS_LOG_H__

#include <memory>
#include <string>
#include <stdint.h>


namespace shs{

// 日志事件
class LogEvent {
public:
    using ptr = std::shared_ptr<LogEvent>;
    LogEvent();
private:
    std::string m_file;         // 文件名
    int32_t m_line = 0;         // 行号
    uint32_t m_threadId = 0;    // 线程号
    uint32_t m_fiberId = 0;     // 协程号
    uint64_t m_time;            // 时间戳
    std::string m_content;      // 内容
};

// 日志级别
enum class LogLevel: uint8_t {
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
};

// 日志格式
class LogFormatter {
};

// 日志输出地
class LogAppender {
public:
    using ptr = std::shared_ptr<LogAppender>;
    virtual ~LogAppender() {}   // 虚析构函数，使得在使用基类的指针析构时也会先调用派生类的析构函数

    void log(LogLevel level, LogEvent::ptr event);
private:
    LogLevel m_level;
};

// 日志器
class Logger {
public:
    using ptr = std::shared_ptr<Logger>;

    Logger(const std::string& name = "root");
    void log(LogLevel level, LogEvent::ptr event);
private:
    std::string m_name;
    LogLevel m_level;
    LogAppender::ptr m_appender;

};

// 输出到控制台的 Appender
class StdoutAppender: public LogAppender {

};
// 输出到文件的 Appender
class FileAppender: public LogAppender {

};
}

#endif