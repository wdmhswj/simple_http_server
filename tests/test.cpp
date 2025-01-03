#include <iostream>
#include "src/log.h"
// #include "src/util.h"

int main(int argc, char** argv) {
    shs::Logger::ptr logger(new shs::Logger);
    logger->addAppender(shs::LogAppender::ptr(new shs::StdoutLogAppender));

    // shs::FileLogAppender::ptr file_appender(new shs::FileLogAppender("./log.txt"));
    // shs::LogFormatter::ptr fmt(new shs::LogFormatter("%d%T%p%T%m%n"));
    // file_appender->setFormatter(fmt);
    // file_appender->setLevel(shs::LogLevel::ERROR);

    // logger->addAppender(file_appender);

    shs::LogEvent::ptr event(new shs::LogEvent(logger, shs::LogLevel::DEBUG, __FILE__, __LINE__, 0, 1, 2, time(0), "thread_2323"));
    event->getSS() << "hello sylar log";
    logger->log(shs::LogLevel::DEBUG, event);
    std::cout << "hello sylar log" << std::endl;

   
    return 0;
}