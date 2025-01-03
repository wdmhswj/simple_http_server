#include <iostream>
#include "../src/_log/Logger.h"
#include "../src/_log/LogAppender.h"
#include "../src/_log/LogEvent.h"
#include "../src/_log/LogLevel.h"

int main(int argc, char** argv) {
    std::cout << "begin" << std::endl;
    shs::Logger::ptr logger(new shs::Logger);
    

    shs::StdoutLogAppender::ptr stdout_appender(new shs::StdoutLogAppender);
    shs::LogFormatter::ptr fmt(new shs::LogFormatter("%d [%p] <%f:%l> (%m)%n"));
    stdout_appender->setFormatter(fmt);
    stdout_appender->setLevel(shs::LogLevel::DEBUG);

    logger->addAppender(stdout_appender);
    // logger->addAppender(file_appender);

    shs::LogEvent::ptr event(new shs::LogEvent(logger, shs::LogLevel::DEBUG, __FILE__, __LINE__, 0, 1, 2, time(0), "thread_2323"));
    event->getSS() << "hello sylar log";    // message 内容
    logger->log(shs::LogLevel::DEBUG, event);
    std::cout << "hello sylar log" << std::endl;

   
    return 0;
}