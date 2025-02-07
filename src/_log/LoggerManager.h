#pragma once 
#include <map>
#include <string>
#include "Logger.h"
#include "../util/singleton.h"
namespace shs {

class LoggerManager
{
private:
    // 日志器容器
    std::map<std::string, Logger::ptr> m_loggers;
    // 主日志器
    Logger::ptr m_root;
public:
    LoggerManager();
    ~LoggerManager();

    // 获取一个名为 name 的 Logger，如果没有则创建
    Logger::ptr getLogger(const std::string& name);
    Logger::ptr getRoot() const { return m_root;}

    YAML::Node toYamlNode();
    std::string toYamlString();
};

using LoggerMgr = SingleTonPtr<LoggerManager>;

}