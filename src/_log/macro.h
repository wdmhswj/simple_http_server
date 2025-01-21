#pragma once
#include "LogLevel.h"
#include "Logger.h"
#include "LogEvent.h"
#include "../util/util.h"
#include <ctime>


// 使用流式方式将日志级别 level 的日志写入到 logger
#define SHS_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        shs::LogEventWrap(shs::LogEvent::ptr(new shs::LogEvent(logger, level, __FILE__, __LINE__, 0, shs::GetThreadIdBySyscall(), shs::GetFiberId(), time(0), "shs::Thread::GetName()"))).getSS()

#define SHS_LOG_DEBUG(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::DEBUG)
#define SHS_LOG_INFO(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::INFO)
#define SHS_LOG_WARN(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::WARN)
#define SHS_LOG_ERROR(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::ERROR_)
#define SHS_LOG_FATAL(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::FATAL)