#include <iostream>
#include "../src/_log/log.h"


int main(int argc, char** argv) {
    shs::Logger::ptr logger(new shs::Logger);   //DEBUG
    auto stdAppender = shs::LogAppender::ptr(new shs::StdoutLogAppender);
    auto fmt_1 = shs::LogFormatter::ptr(new shs::LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    stdAppender->setFormatter(fmt_1);
    logger->addAppender(stdAppender);

    shs::FileLogAppender::ptr file_appender(new shs::FileLogAppender("./log.txt"));
    // shs::LogFormatter::ptr fmt(new shs::LogFormatter("%d%T%p%T%m%n"));
    shs::LogFormatter::ptr fmt(new shs::LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(shs::LogLevel::DEBUG);

    logger->addAppender(file_appender);

    // std::cout << "hello shs log" << std::endl;

    shs::LogEvent::ptr event(new shs::LogEvent(logger, shs::LogLevel::DEBUG, __FILE__, __LINE__, 0, 1, 2, time(0), "thread_2323"));     // 日志事件，管理 logger 对象
    event->getSS() << "hello sylar log";                        // message 内容
    logger->log(shs::LogLevel::ERROR_, event);                   // 向对应的 appender 中打印日志

    // SHS_LOG_INFO(logger) << "test macro";
    // SHS_LOG_ERROR(logger) << "test macro error";

    // SHS_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = shs::LoggerMgr::GetInstance()->getLogger("xx");
    l->addAppender(shs::LogAppender::ptr(new shs::StdoutLogAppender));
    SHS_LOG_INFO(l) << "xxx";
    SHS_LOG_ERROR(l) << "xxx";
    return 0;
}