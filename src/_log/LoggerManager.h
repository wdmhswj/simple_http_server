#pragma once 
#include <map>
#include <string>
#include "Logger.h"
#include "src/util/singleton.h"
#include "src/mutex/mutex.h"

namespace shs {

class LoggerManager
{
public:
    // using MutexType = Mutex;
    using MutexType = SpinLock;
    // using MutexType = CASLock;
    // using MutexType = NullMutex;
    LoggerManager();
    ~LoggerManager();

    // 获取一个名为 name 的 Logger，如果没有则创建
    Logger::ptr getLogger(const std::string& name);
    Logger::ptr getRoot() const {
        MutexType::Lock lock(m_mutex); 
        return m_root;
    }

    YAML::Node toYamlNode();
    std::string toYamlString();
private:
    // 日志器容器
    std::map<std::string, Logger::ptr> m_loggers;
    // 主日志器
    Logger::ptr m_root;
    mutable MutexType m_mutex;
};

using LoggerMgr = SingleTonPtr<LoggerManager>;

}