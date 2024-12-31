#include "log.h"
#include <sstream>
#include <fstream>
#include <iostream>

namespace shs {

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
}
