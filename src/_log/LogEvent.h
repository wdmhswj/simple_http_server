#pragma once

#include <memory>
#include <string>
#include <cstdarg>    // va_list 和可变参数支持
#include <sstream>    // std::stringstream

#include "LogLevel.h"



namespace shs {

class Logger;

// 日志事件
class LogEvent {
public:
    using ptr = std::shared_ptr<LogEvent>;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel level, const std::string& file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string& thread_name);

    const std::string& getFile() const { return m_file; }
    int32_t getLine() const { return m_line;}
    uint32_t getElapse() const { return m_elapse;}
    uint32_t getThreadId() const { return m_threadId;}
    uint32_t getFiberId() const { return m_fiberId;}
    uint64_t getTime() const { return m_time;}
    const std::string& getThreadName() const { return m_threadName;}
    std::string getContent() const { return m_ss.str();}
    std::stringstream& getSS() { return m_ss;}
    std::shared_ptr<Logger> getLogger() const { return m_logger; }
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
    std::shared_ptr<Logger> m_logger;       // 日志器
    LogLevel m_level;           // 日志级别
};

// 日志事件包装器
class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr e);

    ~LogEventWrap();

    LogEvent::ptr getEvent() const { return m_event; }

    // 获取日志内容流
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;
};
}