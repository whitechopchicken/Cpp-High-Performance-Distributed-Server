#include <iostream>
#include "../src/logger.h"
#include "../src/util.h"

int main(int argc, char** argv) {
    yy::Logger::ptr logger(new yy::Logger);
    logger->addAppender(yy::LogAppender::ptr(new yy::StdoutLogAppender));

    yy::FileLogAppender::ptr file_appender(new yy::FileLogAppender("./log.txt"));
    yy::LogFormatter::ptr fmt(new yy::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(yy::LogLevel::ERROR);

    logger->addAppender(file_appender);

    //yy::LogEvent::ptr event(new yy::LogEvent(__FILE__, __LINE__, 0, yy::GetThreadId(), yy::GetFiberId(), time(0)));
    //event->getSS() << "hello yy log";
    //logger->log(yy::LogLevel::DEBUG, event);
    std::cout << "hello yy log" << std::endl;

    // yy_LOG_INFO(logger) << "test macro";
    // yy_LOG_ERROR(logger) << "test macro error";

    // yy_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    // auto l = yy::LoggerMgr::GetInstance()->getLogger("xx");
    // yy_LOG_INFO(l) << "xxx";
    return 0;
}