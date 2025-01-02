#ifndef __SHS_LOG_H__
#define __SHS_LOG_H__

#include <memory>
#include <string>
#include <stdint.h>
#include <list>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

namespace shs{

// 日志级别
enum class LogLevel: uint8_t {
    UNKOWNN = 0, // 未知级别
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
};


// 日志事件
class LogEvent {
public:
    using ptr = std::shared_ptr<LogEvent>;
    LogEvent(Logger::ptr logger, LogLevel level, const std::string& file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string& thread_name);

    const std::string& getFile() const { return m_file; }
    int32_t getLine() const { return m_line;}
    uint32_t getElapse() const { return m_elapse;}
    uint32_t getThreadId() const { return m_threadId;}
    uint32_t getFiberId() const { return m_fiberId;}
    uint64_t getTime() const { return m_time;}
    const std::string& getThreadName() const { return m_threadName;}
    std::string getContent() const { return m_ss.str();}
    std::stringstream& getSS() { return m_ss;}
    Logger::ptr getLogger() const { return m_logger; }
    LogLevel getLevel() const { return m_level; }

    void format(const char* fmt, ...);

    void format(const char* fmt, va_list al);

private:
    std::string m_file;         // 文件名
    int32_t m_line = 0;         // 行号
    uint32_t m_elapse = 0;      // 程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;    // 线程号
    uint32_t m_fiberId = 0;     // 协程号
    uint64_t m_time;            // 时间戳
    std::string m_threadName;   // 线程名称
    std::stringstream m_ss;     // 日志内容流
    Logger::ptr m_logger;       // 日志器
    LogLevel m_level;           // 日志级别
};


// 日志级别辅助类
class LogLevelHelper {
public:
    static std::string to_string(LogLevel level) {
        switch(level) {
            case LogLevel::UNKOWNN: return "UNKOWNN";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }

    static LogLevel from_string(const std::string& str) {
        if (str == "UNKOWNN") return LogLevel::UNKOWNN;
        if (str == "DEBUG") return LogLevel::DEBUG;
        if (str == "INFO") return LogLevel::INFO;
        if (str == "WARN") return LogLevel::WARN;
        if (str == "ERROR") return LogLevel::ERROR;
        if (str == "FATAL") return LogLevel::FATAL;
        throw std::invalid_argument("Invalid log level string");
    }
};


// 日志格式
class LogFormatter {
public:
    using ptr = std::shared_ptr<LogFormatter>;
    /**
     * @brief 构造函数
     * @param[in] pattern 格式模板
     * @details 
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
    LogFormatter(const std::string& pattern);

    std::string format(Logger::ptr logger, LogLevel level, LogEvent::ptr event);
    std::ostream& format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event);

public:
    class FormatItem {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        
        virtual ~FormatItem() {}

        virtual void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) = 0;
    };

    

    bool isError() const { return m_error; }

    const std::string getPattern() const { return m_pattern; }   
private:

    void init();    // 初始化，解析日志模板
    
    // 日志格式模板
    std::string m_pattern;
    // 日志格式解析后格式
    std::vector<FormatItem::ptr> m_items;
    // 是否有错误
    bool m_error = false; 
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