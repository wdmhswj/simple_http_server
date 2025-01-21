#pragma once 
#include "LogLevel.h"
#include "Logger.h"
#include "LogEvent.h"
#include "LogFormatter.h"
#include "LogAppender.h"
#include "LoggerManager.h"
#include "macro.h"
#include "singleton.h"

namespace shs {
    using LoggerMgr = SingleTonPtr<LoggerManager>;
}