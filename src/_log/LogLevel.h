#pragma once

#include <string>
#include <stdint.h>
#include <stdexcept>

namespace shs {

// 日志级别
enum class LogLevel: uint8_t {
    UNKNOW = 0, // 未知级别
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR_ = 4,
    FATAL = 5,
};

// 日志级别辅助类
class LogLevelHelper {
public:
    static std::string to_string(LogLevel level) {
        switch(level) {
            case LogLevel::UNKNOW: return "UNKNOW";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR_: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }

    static LogLevel from_string(const std::string& str) {
        #define XX(level, log_level) if(str == #level) return LogLevel::log_level

        XX(UNKNOW, UNKNOW);
        XX(unknow, UNKNOW);
        XX(DEBUG, DEBUG);
        XX(debug, DEBUG);
        XX(INFO, INFO);
        XX(info, INFO);
        XX(WARN, WARN);
        XX(warn, WARN);
        XX(ERROR, ERROR_);
        XX(error, ERROR_);
        XX(FATAL, FATAL);
        XX(fatal, FATAL);
        #undef XX
        return LogLevel::UNKNOW;
    }
};

}