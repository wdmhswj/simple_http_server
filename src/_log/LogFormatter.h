#pragma once 
#include <memory>
#include <string>
#include <vector>
#include "Logger.h"
#include "LogLevel.h"
#include "LogEvent.h"

namespace shs {

// 日志格式
class LogFormatter {
public:
    using ptr = std::shared_ptr<LogFormatter>;
    
    LogFormatter(const std::string& pattern);

    std::string format(Logger::ptr logger, LogLevel level, LogEvent::ptr event);
    std::ostream& format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event);

    class FormatItem {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        
        virtual ~FormatItem() {}

        virtual void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) = 0;
    };

    

    bool isError() const { return m_error; }

    const std::string& getPattern() const { return m_pattern; }   
private:

    void init();    // 初始化，解析日志模板
    
    // 日志格式模板
    std::string m_pattern;
    // 日志格式解析后格式
    std::vector<FormatItem::ptr> m_items;
    // 是否有错误
    bool m_error = false; 
};

}