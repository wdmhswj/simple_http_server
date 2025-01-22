#include <tuple>
#include <map>
#include <functional>
#include <iostream>
#include <ctime>
#include <cstdlib>    // free
#include <cstdio>     // vasprintf

#include "LogAppender.h"
#include "LogEvent.h"
#include "LogFormatter.h"
#include "Logger.h"
#include "LogLevel.h"
#include "LoggerManager.h"
#include "../util/util.h"

namespace shs {

LogEvent::LogEvent(Logger::ptr logger, LogLevel level, const std::string& file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string& thread_name)
    :m_file(file),
    m_line(line),
    m_elapse(elapse),
    m_threadId(thread_id),
    m_fiberId(fiber_id),
    m_time(time),
    m_threadName(thread_name),
    m_logger(logger),
    m_level(level)
{
}

Logger::Logger(const std::string& name)
    : m_name(name)
    , m_level(LogLevel::DEBUG) {
    // m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}


void Logger::log(LogLevel level, std::shared_ptr<LogEvent> event) {
    // std::cout << "level: " << LogLevelHelper::to_string(level) << std::endl;
    // std::cout << "m_level: " << LogLevelHelper::to_string(m_level) << std::endl;
    if(level>=m_level) {
        for(auto& a: m_appenders) {
            // std::cout << "test1" << std::endl;
            auto self = shared_from_this();
            a->log(self, level, event);
        }
    }
}


// 不同级别额日志打印
void Logger::debug(std::shared_ptr<LogEvent> event) {
    log(LogLevel::DEBUG, event);    
}
void Logger::info(std::shared_ptr<LogEvent> event) {
    log(LogLevel::INFO, event);
}
void Logger::warn(std::shared_ptr<LogEvent> event) {
    log(LogLevel::WARN, event);
}
void Logger::error(std::shared_ptr<LogEvent> event) {
    log(LogLevel::ERROR_, event);
}
void Logger::fatal(std::shared_ptr<LogEvent> event) {
    log(LogLevel::FATAL, event);
}

// 对 Appender 集合的操作
void Logger::addAppender(std::shared_ptr<LogAppender> appender) {
    m_appenders.push_back(appender);
}
void Logger::delAppender(std::shared_ptr<LogAppender> appender) {
    for(auto it=m_appenders.begin(); it!=m_appenders.end(); ++it) {
        if(*it==appender) {
            m_appenders.erase(it);
            break;
        }
    }
}


LogAppender::LogAppender(LogLevel level, LogFormatter::ptr formatter): m_level(level), m_formatter(formatter) {
}



void StdoutLogAppender::log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
    // std::cout << "level: " << LogLevelHelper::to_string(level) << std::endl;
    // std::cout << "m_level: " << LogLevelHelper::to_string(m_level) << std::endl;
    // if(m_formatter==nullptr) 
    //     std::cout << "m_formatter is nullptr" << std::endl;
    if(level >= m_level)
        std::cout << m_formatter->format(logger, level, event) << std::endl;
}

void FileLogAppender::log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
    // std::cout << "FileLogAppender::log\n";
    // std::cout << "level: " << LogLevelHelper::to_string(level) << std::endl;
    // std::cout << "m_level: " << LogLevelHelper::to_string(m_level) << std::endl;
    if(level >= m_level)
        m_filestream << m_formatter->format(logger, level, event) << std::endl;
}

FileLogAppender::FileLogAppender(const std::string& filename): m_filename(filename) {
    this->reopen();
}

bool FileLogAppender::reopen() {
    if(m_filestream) {  // 流是否打开/是否有效
        m_filestream.close();
    }

    m_filestream.open(m_filename);
    return !!m_filestream;
}

LogFormatter::LogFormatter(const std::string& pattern): m_pattern(pattern) {
    // std::cout << "m_pattern: " << m_pattern << std::endl;
    init();
}

std::string LogFormatter::format(Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
    std::stringstream ss;
    // std::cout << "std::string LogFormatter::format"  << std::endl;
    for(auto& i: m_items) {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}


std::ostream& LogFormatter::format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
    for(auto& i: m_items) {
        i->format(os, logger, level, event);
    }
    return os;
}


class MessageFormatItem: public LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};

class LevelFormatItem: public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << LogLevelHelper::to_string(level);
    }
};

class ElapseFormatItem: public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class NameFormatItem: public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << logger->getName();
    }
};

class ThreadIdFormatItem: public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem: public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};

class ThreadNameFormatItem: public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getThreadName();
    }
};

class DateTimeFormatItem: public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S"): m_format(format) {
        if(m_format.empty()) {
            m_format =  "%Y-%m-%d %H:%M:%S";
        }
    }
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        // 获取时间
        time_t t = static_cast<time_t>(event->getTime());
        
        // 转换为本地时间
        struct tm local_tm;
        if(safe_localtime(&t, &local_tm)) {
            // 使用 strftime 格式化时间
            char buffer[80];
            strftime(buffer, 80, m_format.c_str(), &local_tm);
            os << buffer;
        }
        
    }
private:
    std::string m_format;
};

class FilenameFormatItem: public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};

class LineFormatItem: public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem: public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class StringFormatItem: public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str): m_string(str) {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem: public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = ""): m_string(str) {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << "\t";
    }
private:
    std::string m_string;
};


// %xxx %xxx{xxx} %%
void LogFormatter::init() {
    // str, format, type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for(size_t i=0; i<m_pattern.size(); ++i) {
        // 普通字符
        if(m_pattern[i]!='%') {
            nstr.append(1,m_pattern[i]);
            continue;
        }

        
        if((i+1) < m_pattern.size()) {
            if(m_pattern[i+1] == '%') { // %字符
                nstr.append(1, '%');
                continue;
            }
        }

        // 格式化字符
        size_t n = i+1;
        int fmt_status = 0; // 解析状态
        size_t fmt_begin = 0;   // 格式化项起始位置

        std::string str;    // 格式化字符
        std::string fmt;    // 花括号中的格式化项

        while (n<m_pattern.size())
        {
            // 解析 %m 中的 m
            if(fmt_status==0 && (!isalpha(m_pattern[n])) && m_pattern[n]!='{' && m_pattern[n]!='}') {
                str = m_pattern.substr(i+1, n-i-1);
                break;
            }

            // 解析花括号内容
            if(fmt_status==0) {
                if(m_pattern[n]=='{') {
                    str = m_pattern.substr(i+1, n-i-1);
                    fmt_status = 1; // 开始解析花括号中的格式化项
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            }else if(fmt_status==1) {
                if(m_pattern[n]=='}') {
                    fmt = m_pattern.substr(fmt_begin+1, n-fmt_begin-1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }

            ++n;

            // 正好解析到字符串结尾，例如 "%m %d" 中的 %d
            if(n==m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i+1);
                }
            }

        }

        if(fmt_status == 0) {
            if(!nstr.empty()) { // 之前解析到的普通字符串
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;  // 后退一步
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }
    // 存储结尾的普通字符串
    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items = {
        //{"m", [](const std::string& fmt) { return FormatItem::ptr(new MessageFormatItem()); } }
        #define XX(str, C) \
            {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt)); } }
            XX(m, MessageFormatItem),           //m:消息
            XX(p, LevelFormatItem),             //p:日志级别
            XX(r, ElapseFormatItem),            //r:累计毫秒数
            XX(c, NameFormatItem),              //c:日志名称
            XX(t, ThreadIdFormatItem),          //t:线程id
            XX(n, NewLineFormatItem),           //n:换行
            XX(d, DateTimeFormatItem),          //d:时间
            XX(f, FilenameFormatItem),          //f:文件名
            XX(l, LineFormatItem),              //l:行号
            XX(T, TabFormatItem),               //T:Tab
            XX(F, FiberIdFormatItem),           //F:协程id
            XX(N, ThreadNameFormatItem),        //N:线程名称
        #undef XX
    };

    // std::cout << "init(): vec.size()" << vec.size() << std::endl;

    for(auto& i: vec) {
        if(std::get<2>(i)==0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }else{
            auto it = s_format_items.find(std::get<0>(i));
            if(it==s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %"+std::get<0>(i)+">>")));
                m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" <<std::get<2>(i) << ")" << std::endl;
    }

}

LogEventWrap::LogEventWrap(LogEvent::ptr e): m_event(e) {

}

LogEventWrap::~LogEventWrap() {
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

// 获取日志内容流
std::stringstream& LogEventWrap::getSS() {
    return m_event->getSS();
}

void LogEvent::format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char* fmt, va_list al) {
    // char* buf = nullptr;
    // int len = vasprintf(&buf, fmt, al);
    // if(len != -1) {
    //     m_ss << std::string(buf, len);
    //     free(buf);
    // }
    va_list args_copy;
    va_copy(args_copy, al);

    int size = std::vsnprintf(nullptr, 0, fmt, al) + 1; // 获取格式化所需长度
    if (size <= 0) {
        va_end(args_copy);
        return;
    }

    std::vector<char> buffer(size);
    std::vsnprintf(buffer.data(), size, fmt, args_copy);
    va_end(args_copy);

    m_ss << std::string(buffer.data(), size - 1); // 去掉末尾的 '\0'
}

LoggerManager::LoggerManager() {
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
    m_loggers[m_root->getName()] = m_root;
}
LoggerManager::~LoggerManager() {

}

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    auto it = m_loggers.find(name);
    if(it!=m_loggers.end()) {
        return it->second;
    }

    Logger::ptr logger(new Logger(name));
    logger->setRoot(m_root);
    m_loggers[name] = logger;
    return logger;
}



}


