#include"../src/log.h"
#include"../src/util.h"

int main(){
    /*
    yy::Logger::ptr logger(new yy::Logger);
    logger->addAppender(yy::LogAppender::ptr(new yy::StdoutLogAppender));

    yy::FileLogAppender::ptr file_appender(new yy::FileLogAppender("./log.txt"));
    //新建format
    yy::LogFormatter::ptr fmt(new yy::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    
    //设定file_appender的日志级别，只放error以上的日志
    file_appender->setLevel(yy::LogLevel::ERROR);
    logger->addAppender(file_appender);

    YY_LOG_DEBUG(logger)<<"hello";
    YY_LOG_INFO(logger)<<"hello world";
    YY_LOG_ERROR(logger)<<"hello error";

    YY_LOG_FMT_INFO(logger, "test fmt info is%s", "aa");
    */
    auto l = yy::loggerMgr::GetInstance()->getLogger("xx");
    YY_LOG_INFO(l) << "xxxxx";

    return 0;
}