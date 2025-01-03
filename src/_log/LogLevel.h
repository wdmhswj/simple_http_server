#pragma once

#include <string>
#include <stdint.h>
#include <stdexcept>

namespace shs {

// 日志级别
enum class LogLevel: uint8_t {
    UNKOWNN = 0, // 未知级别
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
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

}