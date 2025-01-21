#include <iostream>
#include "../src/_log/Logger.h"
#include "../src/_log/LogAppender.h"
#include "../src/_log/LogEvent.h"
#include "../src/_log/LogLevel.h"
#include "../src/_log/macro.h"

int main(int argc, char** argv) {
    std::cout << "begin" << std::endl;
    shs::Logger::ptr logger(new shs::Logger);           // Logger 类实例对象
    

    shs::StdoutLogAppender::ptr stdout_appender(new shs::StdoutLogAppender);        // 日志输出位置：标准输出

    shs::LogFormatter::ptr fmt(new shs::LogFormatter("%d [%p] <%f:%l> (%m)%n"));    // 日志格式
    stdout_appender->setFormatter(fmt);                                             // appender 设置日志格式
    stdout_appender->setLevel(shs::LogLevel::DEBUG);                                // 设置日志级别


    shs::FileLogAppender::ptr file_appender(new shs::FileLogAppender("./log.txt"));                         // 日志输出到文件
    shs::LogFormatter::ptr fmt_1(new shs::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt_1);
    file_appender->setLevel(shs::LogLevel::ERROR_);


    logger->addAppender(stdout_appender);                                           // 向 logger 中添加日志 appender
    logger->addAppender(file_appender);

    shs::LogEvent::ptr event(new shs::LogEvent(logger, shs::LogLevel::DEBUG, __FILE__, __LINE__, 0, 1, 2, time(0), "thread_2323"));     // 日志事件，管理 logger 对象
    event->getSS() << "hello sylar log";                        // message 内容
    logger->log(shs::LogLevel::ERROR_, event);                   // 向对应的 appender 中打印日志
    // std::cout << "hello sylar log" << std::endl;

    SHS_LOG_DEBUG(logger);
    SHS_LOG_FATAL(logger);
    


    return 0;
}