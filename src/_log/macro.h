#pragma once
#include "LogLevel.h"
#include "Logger.h"
#include "LogEvent.h"
#include "LoggerManager.h"
#include "../util/util.h"
#include <ctime>


// 使用流式方式将日志级别 level 的日志写入到 logger
#define SHS_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        (shs::LogEventWrap(shs::LogEvent::ptr(new shs::LogEvent(logger, level, __FILE__, __LINE__, 0, shs::GetThreadIdBySyscall(), shs::GetFiberId(), time(0), "shs::Thread::GetName()"))).getSS())

#define SHS_LOG_DEBUG(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::DEBUG)
#define SHS_LOG_INFO(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::INFO)
#define SHS_LOG_WARN(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::WARN)
#define SHS_LOG_ERROR(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::ERROR_)
#define SHS_LOG_FATAL(logger) SHS_LOG_LEVEL(logger, shs::LogLevel::FATAL)


// 将格式化字符串fmt写入日志
#define SHS_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        shs::LogEventWrap(std::make_shared<shs::LogEvent>(logger, level, __FILE__, __LINE__, 0, shs::GetThreadIdBySyscall(), shs::GetFiberId(), time(0), "shs::Thread::GetName"))\
        .getEvent()->format(fmt, __VA_ARGS__)

#define SHS_LOG_FMT_DEBUG(logger, fmt, ...) SHS_LOG_FMT_LEVEL(logger, shs::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SHS_LOG_FMT_INFO(logger, fmt, ...) SHS_LOG_FMT_LEVEL(logger, shs::LogLevel::INFO, fmt, __VA_ARGS__)
#define SHS_LOG_FMT_WARN(logger, fmt, ...) SHS_LOG_FMT_LEVEL(logger, shs::LogLevel::WARN, fmt, __VA_ARGS__)
#define SHS_LOG_FMT_ERROR(logger, fmt, ...) SHS_LOG_FMT_LEVEL(logger, shs::LogLevel::ERROR_, fmt, __VA_ARGS__)
#define SHS_LOG_FMT_FATAL(logger, fmt, ...) SHS_LOG_FMT_LEVEL(logger, shs::LogLevel::FATAL, fmt, __VA_ARGS__)

// 获取主日志器
#define SHS_LOG_ROOT() shs::LoggerMgr::GetInstance()->getRoot()
// 获取name的日志器
#define SHS_LOG_NAME(name) shs::LoggerMgr::GetInstance()->getLogger(name)
