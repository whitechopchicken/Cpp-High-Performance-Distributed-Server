#pragma once
#include <string>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <stdarg.h>


namespace yy{

class Logger;   //先申明(为了编译)，当一个类继承自 std::enable_shared_from_this，这个类的对象就可以在内部使用 shared_from_this() 成员函数来获取自身的 std::shared_ptr。
class LoggerManager;

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
    static LogLevel::Level FromString(const std::string& str);  //?这里static后面为啥不加const？

};

//日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t line, uint32_t elapse
            ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
            ,const std::string& thread_name)
            :
            m_file(file),
            m_line(line),
            m_threadId(thread_id),
            m_elapse(elapse),
            m_fiberId(fiber_id),
            m_time(time),
            m_threadName(thread_name),
            m_logger(logger),
            m_level(level){
    }
    const char* getFile()const {return m_file;}
    int32_t getLine()const {return m_line;}
    uint32_t getThreadId()const {return m_threadId;}
    uint32_t getElapse()const {return m_elapse;}
    uint32_t getFiberId()const {return m_fiberId;}
    uint64_t getTime()const {return m_time;}
    std::string getThreadName()const {return m_threadName;}
    std::stringstream& getSS() {return m_ss;}   //这里为啥不加const？会修改成员变量吗？
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
    LogLevel::Level m_level;     //日志等级
};

//日志格式(化)器
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
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);    //提供给append 去输出-->控制台
    std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    //可以根据不同的需求和场景，定制化地输出日志，比如以简洁的格式输出到控制台，以详细的格式输出到文件
public:
    //日志内容格式化----
    class FormatItem{
        public:
            typedef std::shared_ptr<FormatItem>ptr;
            virtual ~FormatItem() {}
            //格式化日志-->流
            virtual void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
    //初始化，解析日志模板
    void init();
    //是否有错
    bool isError()const {return m_error;}
    //返回日志模板
    const std::string getPattern()const {return m_pattern;}
private:
    std::string m_pattern ; //日志格式模板
    std::vector<FormatItem::ptr> m_items;   //日志格式解析后的 格式集合
    bool m_error = false;   //是否有错
}; 

//日志输出目的地      --日志一般输出到控制台 或者 文件，所以LogAppender会有两个子类
class LogAppender{
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    //typedef Spinlock MutexType;

    virtual ~LogAppender(){}
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;//这也有个log(),纯虚函数， 所以它的子类必须实现这个方法
    virtual std::string toYamlString() = 0; //将日志输出目标的配置转成YAML String
    void setFormatter(LogFormatter::ptr val);
    LogFormatter::ptr getFormatter();     //设置、获取m_formatter的接口
    LogLevel::Level getLevel() const { return m_level;}
    void setLevel(LogLevel::Level level) { m_level = level;}

protected:  // 子类需要使用到level ，所以用protected 而不用private 
    LogLevel::Level m_level = LogLevel::DEBUG;          //有个level
    bool m_hasFormatter = false;                        //是否有自己的日志格式器
    LogFormatter::ptr m_formatter;                      //日志格式器
    //MutexType m_mutex;
};

/*输出到控制台的Appender*/
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;  //描述这个方法是从父类继承重载的实现
    std::string toYamlString() override;
};

/*输出到文件的Appender*/
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);       //为啥文件appender有构造函数，而stdoutappender没有==>因为文件需要文件名，所以特意加了一个构造函数
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;

    //重新打开文件，打开成功返回true； 
    bool reopen();
private:
    std::string m_filename;         //文件路径
    std::ofstream m_filestream;     //文件流==>把内容写到文件 m_filestream 中
    uint64_t m_lastTime = 0;
};

//日志事件包装器
class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr e){ m_event = e;}  //传入日志事件e
    ~LogEventWrap();
    LogEvent::ptr getEvent() const { return m_event;}   //获取日志事件
    std::stringstream& getSS(){ return m_event->getSS();}
private:
    LogEvent::ptr m_event;  //日志事件
};

//日志器
class Logger : public std::enable_shared_from_this<Logger>{
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;    
    //typedef Spinlock MutexType; //锁?
    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);   //写日志
    /*一般有以下几种功能*/
    void debug(LogEvent::ptr event);    //写debug级别的日志
    void info(LogEvent::ptr event);     //写info级别的日志
    void warn(LogEvent::ptr event);     //写warn级别的日志
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);    //添加日志目标
    void delAppender(LogAppender::ptr appender);    //删除日志目标
    void clearAppenders();  //清空日志目标
    LogLevel::Level getLevel() const {return m_level;}  //log级别访问接口
    void setLevel(LogLevel::Level val){ m_level = val;} //log级别设置接口 ---不加const，因为它会修改类的成员
    const std::string& getName()const {return m_name;}  //返回日志名称 --这里为啥加const ，以及引用？
    void setFormatter(LogFormatter::ptr val);           //设置日志格式器
    void setFormatter(const std::string& val);          //设置日志格式模板
    LogFormatter::ptr getFormatter()const {return m_formatter;} //获取日志格式器
    /**/
    std::string toYamlString();     //---将日志器的配置转成YAML String
private:    
    std::string m_name;             //日志名称
    LogLevel::Level m_level;        //有个level(只有满足一定级别才让他输出)
    //MutexType m_mutex;              //mutex
    std::list<LogAppender::ptr> m_appenders; //日志目标集合
    LogFormatter::ptr m_formatter;  //日志格式器
    Logger::ptr m_root;             //主格式器---作用？
};

//日志器管理
class LoggerManager {
public:
    //typedef Spinlock MutexType;
    LoggerManager();
    Logger::ptr getLogger(const std::string& name); //传入logger 名字，获取日志器
    void init();                                    //初始化
    Logger::ptr getRoot() const { return m_root;}   //返回主日志器
    std::string toYamlString();                     //将所有的日志器配置转成 YAML String

private:
    //MutexType m_mutex;
    std::map<std::string, Logger::ptr> m_loggers;   //日志器容器，这里应该可以用 unordered_map ，线性查找效率
    Logger::ptr m_root;         //主日志器
};

//日志器管理类单例模式
//typedef yy::Singleton<LoggerManager> LoggerMgr;


/********************************************************************************************************************************/
/*各种FormatItem的子类, Message、Level、Elapse、Name、ThreadId、FiberId、ThreadName、DataTime、Filename、line、NewLine、String、Tab*/
/*只有一个简单的构造函数以及 void format() 函数重写*********************************************************************************/
class MessageFormatItem : public LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string& str = ""){}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getContent();
    }
};
class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(level);
    }
};
class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};
class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLogger()->getName();
    }
};
class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};
class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};
class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadName();
    }
};
class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};
class FilenameFormatItem : public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};
class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};
class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << std::endl;
    }
};
class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str)
        :m_string(str) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};
class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << "\t";
    }
private:
    std::string m_string;
};
/*******************************************************************************************************************************/


}


