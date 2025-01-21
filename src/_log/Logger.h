#pragma once

#include <memory>
#include <string>
#include "LogLevel.h"
#include "list"
namespace shs {

class LogAppender;
class LogEvent;
class LogFormatter;

// 日志器
class Logger: public std::enable_shared_from_this<Logger> {
public:
    using ptr = std::shared_ptr<Logger>;

    Logger(const std::string& name = "root");
    void log(LogLevel level, std::shared_ptr<LogEvent> event);

    // 不同级别额日志打印
    void debug(std::shared_ptr<LogEvent> event);
    void info(std::shared_ptr<LogEvent> event);
    void warn(std::shared_ptr<LogEvent> event);
    void error(std::shared_ptr<LogEvent> event);
    void fatal(std::shared_ptr<LogEvent> event);

    // 对 Appender 集合的操作
    void addAppender(std::shared_ptr<LogAppender> appender);
    void delAppender(std::shared_ptr<LogAppender> appender);

    // 对日志级别的操作
    void setLevel(LogLevel level) { m_level = level; }
    LogLevel getLevel() const { return m_level; }
    const std::string& getName() const { return m_name; }

    void setRoot(Logger::ptr root) { m_root = root; }
private:
    std::string m_name;                         // 日志名称
    LogLevel m_level;                           // 日志级别
    std::shared_ptr<LogFormatter> m_formatter;              // 日志格式器
    // std::shared_ptr<LogAppender> m_appender;
    std::list<std::shared_ptr<LogAppender>> m_appenders;    // Appender 集合
    Logger::ptr m_root;                                     // 主日志器

};


}