#ifndef __SHS_LOG_H__
#define __SHS_LOG_H__

#include <memory>
#include <string>
#include <stdint.h>
#include <list>

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
public:
    using ptr = std::shared_ptr<LogFormatter>;
    std::string format(LogEvent::ptr event);
private:
};

// 日志输出地
class LogAppender {
public:
    using ptr = std::shared_ptr<LogAppender>;
    LogAppender(LogLevel level = LogLevel::DEBUG, LogFormatter::ptr formatter=nullptr);
    virtual ~LogAppender() {}                                   // 虚析构函数，使得在使用基类的指针析构时也会先调用派生类的析构函数

    virtual void log(LogLevel level, LogEvent::ptr event) = 0;  // 纯虚函数，子类必须实现

    // 对输出格式的操作
    void setFormatter(LogFormatter::ptr formatter) { m_formatter = formatter; }
    LogFormatter::ptr getFormatter() const { return m_formatter; }

    // 对日志级别的操作
    void setLevel(LogLevel level) { m_level = level; }
    LogLevel getLevel() const { return m_level; }
protected:                                                      // 保护模式：public继承的子类还可以直接访问
    LogLevel m_level;                                           // 日志级别
    LogFormatter::ptr m_formatter;                              // 输出格式
};

// 日志器
class Logger {
public:
    using ptr = std::shared_ptr<Logger>;

    Logger(const std::string& name = "root");
    void log(LogLevel level, LogEvent::ptr event);

    // 不同级别额日志打印
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    // 对 Appender 集合的操作
    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);

    // 对日志级别的操作
    void setLevel(LogLevel level) { m_level = level; }
    LogLevel getLevel() const { return m_level; }
private:
    std::string m_name;                         // 日志名称
    LogLevel m_level;                           // 日志级别
    // LogAppender::ptr m_appender;
    std::list<LogAppender::ptr> m_appenders;    // Appender 集合

};

// 输出到控制台的 Appender
class StdoutAppender: public LogAppender {
public:
    using ptr = std::shared_ptr<StdoutAppender>;
    
    void log(LogLevel level, LogEvent::ptr event) override;
private:
};
// 输出到文件的 Appender
class FileAppender: public LogAppender {
public:
    using ptr = std::shared_ptr<FileAppender>;

    FileAppender(std::string& filename);

    void log(LogLevel level, LogEvent::ptr event) override;

    // 重新打开文件，文件打开成功返回 true
    bool reopen();
private:
    std::string m_filename;     // 文件名
    std::ofstream m_filestream; // 文件输出流
};
}

#endif