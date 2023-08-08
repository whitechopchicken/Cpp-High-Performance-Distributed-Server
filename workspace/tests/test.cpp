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

    yy_LOG_INFO(logger) << "test macro";
    yy_LOG_ERROR(logger) << "test macro error";

    yy_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = yy::LoggerMgr::GetInstance()->getLogger("xx");
    yy_LOG_INFO(l) << "xxx";
    return 0;
}

// int main(int argc, char** argv){
//     yy::Logger::ptr myLogger(new yy::Logger);   //用智能指针来操作class 对象
//     myLogger->addAppender(yy::LogAppender::ptr(new yy::StdoutLogAppender)); //给日志器mylogger 添加一个 输出到控制台的 日志目标 

//     yy::LogEvent::ptr myEvent(new yy::LogEvent);
//     //yy::LogEvent::ptr myEvent(new yy::LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));// LogEvent的传参构造里有个logger，logger传参构造里又有个event 什么逻辑啊

//     /* LogEvent参数有很多：   9个参数，这里为啥只有6个呢   ==> log.h 开头的宏  ，使用格式化方式将日志级别level的日志写入到logger ==> 没用！
//     //__FILE__是c++定义的宏，获取当前文件名
//     LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
//             ,const char* file, int32_t line, uint32_t elapse
//             ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
//             ,const std::string& thread_name)   
//     */
//    myEvent->getSS() << "hello yy log" ; //手动给事件添加,即 用m_ss 这个 stringstream对象来读入一段字符串

//    myLogger->log(yy::LogLevel::Level::DEBUG, myEvent);// 调用 log() 函数，写入日志， 其中，参数是 日志等级&日志事件
//    std::cout << "hello yy log" << std::endl;
   
//     return 0;
// }