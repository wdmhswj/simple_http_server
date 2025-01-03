#include <memory>
#include <string>
#include "LogLevel.h"
#include "LogEvent.h"

// 日志器
class Logger: public std::enable_shared_from_this<Logger> {
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
    const std::string& getName() const { return m_name; }
private:
    std::string m_name;                         // 日志名称
    LogLevel m_level;                           // 日志级别
    // LogAppender::ptr m_appender;
    std::list<LogAppender::ptr> m_appenders;    // Appender 集合

};

