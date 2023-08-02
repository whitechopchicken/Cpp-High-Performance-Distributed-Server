#pragma once
#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>

namespace yy{

//实现日志级别转换成string
static const char* LogLevel::ToString(LogLevel::Level level){
    switch (level)
    {

    case 
        break;
    
    default:
        break;
    }
}

//日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t line, uint32_t elapse
            ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
            ,const std::string& thread_name);

    const char* getFile()const {return m_file;}
    int32_t getLine()const {return m_line;}
    uint32_t getThreadId()const {return m_threadId;}
    uint32_t getElapse()const {return m_elapse;}
    uint32_t getFiberId()const {return m_fiberId;}
    uint64_t getTime()const {return m_time;}
    std::string getThreadName()const {return m_threadName;}
    std::stringstream getSS()const {return m_ss;}
    std::string getContent()const {return m_ss.str();}  //返回日志内容
    std::shared_ptr<Logger> getLogger()const {return m_logger;}
    LogLevel::Level getLevel()const {return m_level;}

    void format(const char* fmt, ...);  //格式化写入日志内容------使用 ... 语法来接受可变数量的参数。
    void format(const char* fmt, va_list al);//格式化写入日志内容----另一种可变参数方法

private:
    const char* m_file = nullptr;   //文件名
    int32_t m_line = 0;             //行号
    uint32_t m_threadId = 0;        //线程id
    uint32_t m_elapse = 0;          //程序启动开始到现在的毫秒数 -----(这里用到内存对齐技巧)
    uint32_t m_fiberId = 0;         //协程id
    uint64_t m_time;                //时间戳
    std::string m_threadName;       //线程名称
    std::stringstream m_ss;         //日志流内容-->m_ss.str() 就是日志内容
    std::shared_ptr<Logger> m_logger;//日志器       
    LogLevel::LogLevel m_level;     //日志等级
};

//日志级别
class LogLevel{
public:
    enum Level{
        UNKNOW = 0,
        DEBUG =1 , 
        INFO = 2 , 
        WARN = 3 , 
        ERROR = 4 , 
        FATAL = 5 
    };
    //将日志级别转换成文本输出==>  static 成员函数 
    static const char* ToString(LogLevel::Level level);
    //将文本转换成日志级别==>
    static const LogLevel::Level FromString(const std::string& str);

};

//日志器
class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;    //智能指针，方便内存管理
    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);       //这有个log()
    /*一般有以下几种功能*/
    void debug(LogEvent::ptr event);    //写debug级别的日志
    void info(LogEvent::ptr event);     //写info级别的日志
    void warn(LogEvent::ptr event);     //写warn级别的日志
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);    //添加日志目标
    void delAppender(LogAppender::ptr appender);    //删除日志目标
    void clearAppenders(LogAppender::ptr appender);  //清空日志目标
    LogLevel::Level getLevel() const {return m_level;}  //log级别访问接口
    void setLevel(logLevel::Level val){ m_level = val;} //log级别设置接口 ---不加const，因为它会修改类的成员

private:    
    std::string m_name;             //日志别名
    LogLevel::Level m_level;        //有个level(只有满足一定级别才让他输出)
    std::list<LogAppender> m_appenders ;                //输出到的Appender集合 , 一个列表
};

/*日志格式器*/  
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
        /**
     *  构造函数
     *  pattern 格式模板
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
    LogFormatter(const std::string& pattern);
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);    //提供给append 去输出
    std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    //可以根据不同的需求和场景，定制化地输出日志，比如以简洁的格式输出到控制台，以详细的格式输出到文件
public:
    //日志内容格式化----
    class FormatItem{
        public:
            typedef std::shared_ptr<FormatItem>ptr;
            virtual ~FormatItem() {}
            virtual void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
    //初始化，解析日志模板
    void init();
    //是否有错
    bool isError()const {return m_error;}
private:
    std::string m_pattern ; //日志格式模板
    std::vector<FormatItem::ptr> m_items;   //日志格式解析后的格式
    bool m_error = false;   //是否有错
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


