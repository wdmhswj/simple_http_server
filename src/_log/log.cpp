#include <tuple>
#include <map>
#include <functional>
#include <iostream>
#include <ctime>
#include <cstdlib>    // free
#include <cstdio>     // vasprintf

#include "LogAppender.h"
#include "LogEvent.h"
#include "LogFormatter.h"
#include "Logger.h"
#include "LogLevel.h"
#include "LoggerManager.h"
#include "src/util/util.h"
#include "src/config/config.h"

namespace shs {

LogEvent::LogEvent(Logger::ptr logger, LogLevel level, const std::string& file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string& thread_name)
    :m_file(file),
    m_line(line),
    m_elapse(elapse),
    m_threadId(thread_id),
    m_fiberId(fiber_id),
    m_time(time),
    m_threadName(thread_name),
    m_logger(logger),
    m_level(level)
{
    // std::cout << "LogEvent::LogEvent" << std::endl;
}

Logger::Logger(const std::string& name)
    : m_name(name)
    , m_level(LogLevel::DEBUG) {
    // m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}


void Logger::log(LogLevel level, std::shared_ptr<LogEvent> event) {
    // std::cout << "level: " << LogLevelHelper::to_string(level) << std::endl;
    // std::cout << "m_level: " << LogLevelHelper::to_string(m_level) << std::endl;
    // std::cout << "Logger::log" << std::endl;
    if(level>=m_level) {
        if(!m_appenders.empty()) {
            MutexType::Lock lock(m_mutex);
            for(auto& a: m_appenders) {
                // std::cout << "test1" << std::endl;
                auto self = shared_from_this();
                a->log(self, level, event);
            }
        } else if(m_root) {
            m_root->log(level, event);  // 若 m_appenders 为空，则调用 m_root->log()
        }
    }
}


// 不同级别额日志打印
void Logger::debug(std::shared_ptr<LogEvent> event) {
    log(LogLevel::DEBUG, event);    
}
void Logger::info(std::shared_ptr<LogEvent> event) {
    log(LogLevel::INFO, event);
}
void Logger::warn(std::shared_ptr<LogEvent> event) {
    log(LogLevel::WARN, event);
}
void Logger::error(std::shared_ptr<LogEvent> event) {
    log(LogLevel::ERROR_, event);
}
void Logger::fatal(std::shared_ptr<LogEvent> event) {
    log(LogLevel::FATAL, event);
}

// 对 Appender 集合的操作
void Logger::addAppender(std::shared_ptr<LogAppender> appender) {
    MutexType::Lock lock(m_mutex);
    m_appenders.push_back(appender);
}
void Logger::delAppender(std::shared_ptr<LogAppender> appender) {
    MutexType::Lock lock(m_mutex);
    for(auto it=m_appenders.begin(); it!=m_appenders.end(); ++it) {
        if(*it==appender) {
            m_appenders.erase(it);
            break;
        }
    }
}


LogAppender::LogAppender(LogLevel level, LogFormatter::ptr formatter): m_level(level), m_formatter(formatter) {
}



void StdoutLogAppender::log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
    // std::cout << "level: " << LogLevelHelper::to_string(level) << std::endl;
    // std::cout << "m_level: " << LogLevelHelper::to_string(m_level) << std::endl;
    // if(m_formatter==nullptr) 
    //     std::cout << "m_formatter is nullptr" << std::endl;
    // std::cout << "StdoutLogAppender::log" << std::endl;
    if(level >= m_level) {
        MutexType::Lock lock(m_mutex);
        std::cout << m_formatter->format(logger, level, event) << std::endl;
    }
}

YAML::Node StdoutLogAppender::toYamlNode() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "StdoutLogAppender";
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevelHelper::to_string(m_level);
    }
    if(m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    return node;
}
std::string StdoutLogAppender::toYamlString() {
    std::stringstream ss;
    ss << toYamlNode();
    return ss.str();
}

YAML::Node FileLogAppender::toYamlNode() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "FileLogAppender";
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevelHelper::to_string(m_level);
    }
    if(m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    node["file"] = m_filename;
    return node;
}
std::string FileLogAppender::toYamlString() {
    std::stringstream ss;
    ss << toYamlNode();
    return ss.str();
}


void FileLogAppender::log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
    // std::cout << "FileLogAppender::log\n";
    // std::cout << "level: " << LogLevelHelper::to_string(level) << std::endl;
    // std::cout << "m_level: " << LogLevelHelper::to_string(m_level) << std::endl;
    if(level >= m_level) {
        uint64_t now = event->getTime();
        // uint64_t now = time(0);
        if(now >= (m_lastTime+3)) {
            reopen();
            m_lastTime = now;
        }
        MutexType::Lock lock(m_mutex);
        // m_filestream << m_formatter->format(logger, level, event) << std::endl;
        if(!m_formatter->format(m_filestream, logger, level, event)) {
            std::cout << "error" << std::endl;
        }
    }
}

FileLogAppender::FileLogAppender(const std::string& filename): m_filename(filename) {
    this->reopen();
}

bool FileLogAppender::reopen() {
    MutexType::Lock lock(m_mutex);
    // std::cout << "reopen" << std::endl;
    if(m_filestream) {  // 流是否打开/是否有效
        m_filestream.close();
    }

    m_filestream.open(m_filename, std::ios::app);
    return !!m_filestream;
}

LogFormatter::LogFormatter(const std::string& pattern): m_pattern(pattern) {
    // std::cout << "m_pattern: " << m_pattern << std::endl;
    init();
}

std::string LogFormatter::format(Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
    
    std::stringstream ss;
    // std::cout << "std::string LogFormatter::format"  << std::endl;
    for(auto& i: m_items) {
        i->format(ss, logger, level, event);
    }
    // std::cout << "std::string LogFormatter::format return"  << std::endl;
    return ss.str();
}


std::ostream& LogFormatter::format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
    for(auto& i: m_items) {
        i->format(os, logger, level, event);
    }
    return os;
}


class MessageFormatItem: public LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        // std::cout << "MessageFormatItem::format" << std::endl;
        os << event->getContent();
    }
};

class LevelFormatItem: public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << LogLevelHelper::to_string(level);
    }
};

class ElapseFormatItem: public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class NameFormatItem: public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << logger->getName();
    }
};

class ThreadIdFormatItem: public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem: public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};

class ThreadNameFormatItem: public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getThreadName();
    }
};

class DateTimeFormatItem: public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S"): m_format(format) {
        if(m_format.empty()) {
            m_format =  "%Y-%m-%d %H:%M:%S";
        }
    }
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        // 获取时间
        time_t t = static_cast<time_t>(event->getTime());
        
        // 转换为本地时间
        struct tm local_tm;
        if(safe_localtime(&t, &local_tm)) {
            // 使用 strftime 格式化时间
            char buffer[80];
            strftime(buffer, 80, m_format.c_str(), &local_tm);
            os << buffer;
        }
        
    }
private:
    std::string m_format;
};

class FilenameFormatItem: public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};

class LineFormatItem: public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem: public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class StringFormatItem: public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str): m_string(str) {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem: public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = ""): m_string(str) {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << "\t";
    }
private:
    std::string m_string;
};


// %xxx %xxx{xxx} %%
void LogFormatter::init() {
    // str, format, type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for(size_t i=0; i<m_pattern.size(); ++i) {
        // 普通字符
        if(m_pattern[i]!='%') {
            nstr.append(1,m_pattern[i]);
            continue;
        }

        
        if((i+1) < m_pattern.size()) {
            if(m_pattern[i+1] == '%') { // %字符
                nstr.append(1, '%');
                continue;
            }
        }

        // 格式化字符
        size_t n = i+1;
        int fmt_status = 0; // 解析状态
        size_t fmt_begin = 0;   // 格式化项起始位置

        std::string str;    // 格式化字符
        std::string fmt;    // 花括号中的格式化项

        while (n<m_pattern.size())
        {
            // 解析 %m 中的 m
            if(fmt_status==0 && (!isalpha(m_pattern[n])) && m_pattern[n]!='{' && m_pattern[n]!='}') {
                str = m_pattern.substr(i+1, n-i-1);
                break;
            }

            // 解析花括号内容
            if(fmt_status==0) {
                if(m_pattern[n]=='{') {
                    str = m_pattern.substr(i+1, n-i-1);
                    fmt_status = 1; // 开始解析花括号中的格式化项
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            }else if(fmt_status==1) {
                if(m_pattern[n]=='}') {
                    fmt = m_pattern.substr(fmt_begin+1, n-fmt_begin-1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }

            ++n;

            // 正好解析到字符串结尾，例如 "%m %d" 中的 %d
            if(n==m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i+1);
                }
            }

        }

        if(fmt_status == 0) {
            if(!nstr.empty()) { // 之前解析到的普通字符串
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;  // 后退一步
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }
    // 存储结尾的普通字符串
    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items = {
        //{"m", [](const std::string& fmt) { return FormatItem::ptr(new MessageFormatItem()); } }
        #define XX(str, C) \
            {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt)); } }
            XX(m, MessageFormatItem),           //m:消息
            XX(p, LevelFormatItem),             //p:日志级别
            XX(r, ElapseFormatItem),            //r:累计毫秒数
            XX(c, NameFormatItem),              //c:日志名称
            XX(t, ThreadIdFormatItem),          //t:线程id
            XX(n, NewLineFormatItem),           //n:换行
            XX(d, DateTimeFormatItem),          //d:时间
            XX(f, FilenameFormatItem),          //f:文件名
            XX(l, LineFormatItem),              //l:行号
            XX(T, TabFormatItem),               //T:Tab
            XX(F, FiberIdFormatItem),           //F:协程id
            XX(N, ThreadNameFormatItem),        //N:线程名称
        #undef XX
    };

    // std::cout << "init(): vec.size()" << vec.size() << std::endl;

    for(auto& i: vec) {
        if(std::get<2>(i)==0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }else{
            auto it = s_format_items.find(std::get<0>(i));
            if(it==s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %"+std::get<0>(i)+">>")));
                m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" <<std::get<2>(i) << ")" << std::endl;
    }

}

LogEventWrap::LogEventWrap(LogEvent::ptr e): m_event(e) {
    // std::cout << "LogEventWrap::LogEventWrap" << std::endl;
}

LogEventWrap::~LogEventWrap() {
    // std::cout << "LogEventWrap::~LogEventWrap" << std::endl;
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

// 获取日志内容流
std::stringstream& LogEventWrap::getSS() {
    // std::cout << "LogEventWrap::getSS" << std::endl;
    return m_event->getSS();
}

void LogEvent::format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char* fmt, va_list al) {
    // char* buf = nullptr;
    // int len = vasprintf(&buf, fmt, al);
    // if(len != -1) {
    //     m_ss << std::string(buf, len);
    //     free(buf);
    // }
    va_list args_copy;
    va_copy(args_copy, al);

    int size = std::vsnprintf(nullptr, 0, fmt, al) + 1; // 获取格式化所需长度
    if (size <= 0) {
        va_end(args_copy);
        return;
    }

    std::vector<char> buffer(size);
    std::vsnprintf(buffer.data(), size, fmt, args_copy);
    va_end(args_copy);

    m_ss << std::string(buffer.data(), size - 1); // 去掉末尾的 '\0'
}

LoggerManager::LoggerManager() {
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
    m_loggers[m_root->getName()] = m_root;
}
LoggerManager::~LoggerManager() {

}

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    MutexType::Lock lock(m_mutex); 
    // std::cout << "getLogger(): name=" << name << std::endl;
    // std::cout << "before getLogger() m_loggers.size()=" << m_loggers.size() << std::endl;
    auto it = m_loggers.find(name);
    if(it!=m_loggers.end()) {
        return it->second;
    }

    Logger::ptr logger(new Logger(name));
    logger->setRoot(m_root);                // 设置logger对象的root为LoggerManager的m_root
    m_loggers[name] = logger;               // 注册 logger
    // std::cout << "after getLogger() m_loggers.size()=" << m_loggers.size() << std::endl;
    return logger;
}

YAML::Node LoggerManager::toYamlNode() {
    MutexType::Lock lock(m_mutex); 
    YAML::Node node;
    // std::cout << "m_loggers.size()=" << m_loggers.size() << std::endl;
    for(auto& i: m_loggers) {
        node.push_back(i.second->toYamlNode());
    }
    return node;
}
std::string LoggerManager::toYamlString() {
    std::stringstream ss;
    ss << toYamlNode();
    return ss.str();
}
struct LogAppenderDefine {
    int type = 0; //1 File, 2 Stdout
    LogLevel level = LogLevel::UNKNOW;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const {
        return type == oth.type
            && level == oth.level
            && formatter == oth.formatter
            && file == oth.file;
    }
};

struct LogDefine {
    std::string name;
    LogLevel level = LogLevel::UNKNOW;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const {
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == appenders;
    }

    bool operator<(const LogDefine& oth) const {
        return name < oth.name;
    }

    bool isValid() const {
        return !name.empty();
    }
};

// Lexical_cast: LogDefine 与 std::string 相互的之间转换的偏特化
template<>
class Lexical_cast<std::string, LogDefine> {
public:
    LogDefine operator()(const std::string& f) {
        // std::cout << "Lexical_cast<std::string, LogDefine> operator()" << std::endl; 
        YAML::Node node = YAML::Load(f);
        LogDefine ld;
        // name
        if(!node["name"].IsDefined()) {
            std::cout << "log config error: name is null, " << node << std::endl;
            throw std::logic_error("log config name is null");
        }
        ld.name = node["name"].as<std::string>();
        // level
        ld.level = LogLevelHelper::from_string(node["level"].IsDefined() ? node["level"].as<std::string>() : "");
        // formatter: 取消
        // appenders
        if(node["appenders"].IsDefined()) {
            for(size_t i=0; i<node["appenders"].size(); ++i) {
                auto a = node["appenders"][i];
                
                // type
                if(!a["type"].IsDefined()) {
                    std::cout << "log config error: appender type is null, " << a << std::endl;
                    continue;
                }
                std::string type = a["type"].as<std::string>();
                LogAppenderDefine lad;
                
                if(type == "FileLogAppender") {
                    lad.type = 1;
                    if(!a["file"].IsDefined()) {
                        std::cout << "log config error: fileappender file is null" << a << std::endl;
                        continue;
                    }
                    lad.file = a["file"].as<std::string>();
                } else if(type == "StdoutLogAppender") {
                    lad.type = 2;
                } else {
                    std::cout << "log config error: appender type is invalid, " << a << std::endl;
                }
                
                // level: 先使用 logger 的 level
                lad.level = a["level"].IsDefined() ? LogLevelHelper::from_string(node["level"].as<std::string>()) : ld.level;

                // formatter
                if(a["formatter"].IsDefined())
                    lad.formatter = a["formatter"].as<std::string>();
                
                ld.appenders.push_back(lad);

            }
        }
        return ld;
    }
};

template<>
class Lexical_cast<LogDefine, std::string> {
public:
    std::string operator()(const LogDefine& f) {
        YAML::Node node;
        node["name"] = f.name;
        if(f.level != LogLevel::UNKNOW) {
            node["level"] = LogLevelHelper::to_string(f.level);
        }
        // formatter 取消
        // appenders
        for(auto& a: f.appenders) {
            YAML::Node na;
            if(a.type == 1) {
                na["type"] = "FileLogAppender";
                na["file"] = a.file;
            } else if(a.type == 2) {
                na["type"] = "StdoutLogAppender";
            }
            // level
            if(a.level != LogLevel::UNKNOW) {
                na["level"] = LogLevelHelper::to_string(a.level);
            }
            // formatter
            if(!a.formatter.empty()) {
                na["formatter"] = a.formatter;
            }
            node["appenders"].push_back(na);
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

ConfigVar<std::set<LogDefine>>::ptr g_log_defines = Config::Lookup(std::set<LogDefine>(), "logs", "logs config");

struct LogIniter
{
    LogIniter() {
        // 添加的此回调函数实现对 LoggerMgr 中的 loggers 的 添加/修改/删除，借助 SHS_LOG_NAME()
        g_log_defines->addListener([](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value) {
            SHS_LOG_INFO(SHS_LOG_ROOT()) << "on_logger_conf_changed";
            for(auto& i: new_value) {
                // 在旧配置中查找
                auto it = old_value.find(i);
                Logger::ptr logger;
                if(it!=old_value.end()) {
                    // 修改
                    // 是否相等需要修改
                    if(i==*it)
                        continue;
                    logger = SHS_LOG_NAME(i.name);
                } else {
                    // 新增
                    logger = SHS_LOG_NAME(i.name);
                }
                // level
                logger->setLevel(i.level);
                // formatter: 取消
                // appender
                logger->clearAppenders();   // 先清空
                // std::cout << "after clearAppenders() logger->appeders.size()=" << logger->getAppendersSize() << std::endl;
                // std::cout << "i.appenders.size()=" << i.appenders.size() << std::endl;
                for(auto& a: i.appenders) {
                    LogAppender::ptr ap;
                    if(a.type==1) {
                        ap.reset(new FileLogAppender(a.file));
                    } else if(a.type==2) {
                        ap.reset(new StdoutLogAppender);
                    } else {
                        continue;
                    }
                    // appender: level
                    ap->setLevel(a.level);
                    // appender: formatter
                    if(!a.formatter.empty()) {
                        LogFormatter::ptr fmt = std::make_shared<LogFormatter>(a.formatter);
                        if(!fmt->isError()) {
                            ap->setFormatter(fmt);
                        } else {
                            std::cout << "log.name=" << i.name << " appender type=" << a.type << " formatter=" << a.formatter << " is invalid" << std::endl;
                        }
                    }
                    logger->addAppender(ap);
                }
                // std::cout << "after for loop logger->appeders.size()=" << logger->getAppendersSize() << std::endl;
                // std::cout << "logger.name=" << logger->getName() << std::endl;
            }

            // 删除
            for(auto& i: old_value) {
                auto it = new_value.find(i);
                if(it==new_value.end()) {
                    // 删除logger
                    auto logger = SHS_LOG_NAME(i.name);
                    logger->setLevel(LogLevel::UNKNOW);
                    logger->clearAppenders();
                }
            }
        });
    }
};

static LogIniter __log_init;


YAML::Node Logger::toYamlNode() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["name"] = m_name;
    if(m_level != LogLevel::UNKNOW)
        node["level"] = LogLevelHelper::to_string(m_level);
    for(auto& a: m_appenders) {
        node["appenders"].push_back(a->toYamlNode());
    }
    return node;
}
std::string Logger::toYamlString() {
    std::stringstream ss;
    ss << toYamlNode();
    return ss.str();
}
}


