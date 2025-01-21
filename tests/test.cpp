#include <iostream>
#include "../src/_log/log.h"


int main(int argc, char** argv) {
    shs::Logger::ptr logger(new shs::Logger);   //DEBUG
    logger->addAppender(shs::LogAppender::ptr(new shs::StdoutLogAppender));

    shs::FileLogAppender::ptr file_appender(new shs::FileLogAppender("./log.txt"));
    shs::LogFormatter::ptr fmt(new shs::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(shs::LogLevel::ERROR_);

    logger->addAppender(file_appender);

    std::cout << "hello shs log" << std::endl;

    // SHS_LOG_INFO(logger) << "test macro";
    // SHS_LOG_ERROR(logger) << "test macro error";

    // SHS_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = shs::LoggerMgr::GetInstance()->getLogger("xx");    // 单例模式返回 static 实例对象
    l->addAppender(shs::LogAppender::ptr(new shs::StdoutLogAppender));
    SHS_LOG_INFO(l) << "xxx";
    SHS_LOG_ERROR(l) << "xxx";
    return 0;
}