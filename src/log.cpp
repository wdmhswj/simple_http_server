#include "log.h"
#include <tuple>

namespace shs {

LogEvent::LogEvent(Logger::ptr logger, LogLevel level, const std::string& file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string& thread_name)
:   m_logger(logger),
    m_level(level),
    m_file(file),
    m_line(line),
    m_elapse(elapse),
    m_threadId(thread_id),
    m_fiberId(fiber_id),
    m_time(time),
    m_threadName(thread_name)
{
}

Logger::Logger(const std::string& name = "root"): m_name(name) {

}
void Logger::log(LogLevel level, LogEvent::ptr event) {
    if(level>=m_level) {
        for(auto& a: m_appenders) {
            a->log(level, event);
        }
    }
}


// 不同级别额日志打印
void Logger::debug(LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);    
}
void Logger::info(LogEvent::ptr event) {
    log(LogLevel::INFO, event);
}
void Logger::warn(LogEvent::ptr event) {
    log(LogLevel::WARN, event);
}
void Logger::error(LogEvent::ptr event) {
    log(LogLevel::ERROR, event);
}
void Logger::fatal(LogEvent::ptr event) {
    log(LogLevel::FATAL, event);
}

// 对 Appender 集合的操作
void Logger::addAppender(LogAppender::ptr appender) {
    m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender) {
    for(auto it=m_appenders.begin(); it!=m_appenders.end(); ++it) {
        if(*it==appender) {
            m_appenders.erase(it);
            break;
        }
    }
}


LogAppender::LogAppender(LogLevel level, LogFormatter::ptr formatter): m_level(level), m_formatter(formatter) {

}



void StdoutAppender::log(LogLevel level, LogEvent::ptr event) {
    if(level >= m_level)
        std::cout << m_formatter->format(event) << std::endl;
}

void FileAppender::log(LogLevel level, LogEvent::ptr event) {
    if(level >= m_level)
        m_filestream << m_formatter->format(event) << std::endl;
}

FileAppender::FileAppender(std::string& filename): m_filename(filename) {

}

bool FileAppender::reopen() {
    if(m_filestream) {  // 流是否打开/是否有效
        m_filestream.close();
    }

    m_filestream.open(m_filename);
    return !!m_filestream;
}

LogFormatter::LogFormatter(const std::string& pattern): m_pattern(pattern) {
    init();
}

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
                    str = m_pattern.substr(i+1, n-i-i);
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
}


std::string LogFormatter::format(Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
    std::stringstream ss;
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
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};

class LevelFormatItem: public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << LogLevelHelper::to_string(level);
    }
};


}


