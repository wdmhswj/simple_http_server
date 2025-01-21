#pragma once 
#include <map>
#include <string>
#include "Logger.h"

namespace shs {

class LoggerManager
{
private:
    // 日志器容器
    std::map<std::string, Logger::ptr> m_loggers;
    // 主日志器
    Logger::ptr m_root;
public:
    LoggerManager(/* args */);
    ~LoggerManager();

    Logger::ptr getLogger(const std::string& name);
    Logger::ptr getRoot() const { return m_root;}

};



}