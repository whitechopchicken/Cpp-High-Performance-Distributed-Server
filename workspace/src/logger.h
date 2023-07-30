#pragma once
#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>

namespace yy{

//日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();

private:
    const char* m_file = nullptr;
    int32_t m_line = 0;         //文件名
    uint32_t m_threadId = 0;    //行号
    uint32_t m_elapse = 0;      //程序启动开始到现在的毫秒数 -----(这里用到内存对齐技巧)
    uint32_t m_fiberId = 0;     //线程id
    uint64_t m_time;            //协程id
    std::string m_content;      //时间戳
    std::string m_content;
};

//日志级别
class LogLevel{
public:
    LogLevel();
    enum Level{
        DEBUG =1 , 
        INFO = 2 , 
        WARN = 3 , 
        ERROR = 4 , 
        FATAL = 5 
    };
};

//日志器
class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;    //只能指针，方便内存管理
    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);       //这有个log()
    /*一般有以下几种功能*/
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);    //添加
    void delAppender(LogAppender::ptr appender);    //删除
    LogLevel::Level getLevel() const {return m_level;}  //log级别访问接口
    void setLevel(logLevel::Level val){ m_level = val;} //log级别设置接口

private:    
    std::string m_name;             //日志别名
    LogLevel::Level m_level;        //有个level(只有满足一定级别才让他输出)
    std::list<LogAppender> m_appenders ;                //输出到的Appender集合 , 一个列表
};



/*日志格式器*/  
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    
    std::string format(LogEvent::ptr event);    //提供给append 去输出
}; 


/*日志输出目的地*/   //------------------------------------------------------- 日志一般输出到控制台 或者 文件，所以LogAppender会有两个子类
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){}

    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;       //这也有个log(),纯虚函数， 所以它的子类必须实现这个方法

    void setFormatter(LogFormatter::ptr val){m_formatter = val;}
    LogFormatter getFormatter() const {return m_formatter;}     //设置、获取m_formatter的接口

protected:  // 子类需要使用到level ，所以用protected 而不用private 
    LogLevel::Level m_level;        //有个level
    LogFormatter::ptr m_formatter;
    
};
/*输出到控制台的Appender*/
class stdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<stdoutLogAppender> ptr;
    void log(LogLevel::Level level, LogEvent::ptr event) override;  //描述这个方法是从父类继承重载的实现
};

/*定义输出到文件的Appender*/
class FileLogAppender : public LogAppender{
public:
    typedef std:: shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);       //为啥文件appender有构造函数，而stdoutappender没有==>因为文件需要文件名，所以特意加了一个构造函数
    void log(LogLevel::Level level, LogEvent::ptr event) override;

    //重新打开文件，打开成功返回true； 
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;     //文件一般用ofstream ,用stream的方式去写
};


}


