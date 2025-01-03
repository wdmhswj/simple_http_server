#pragma once

#include <stdint.h>

// 日志级别
enum class LogLevel: uint8_t {
    UNKOWNN = 0, // 未知级别
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
};
