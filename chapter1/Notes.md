# 日志模块  
## 概述  
&emsp;用于格式化输出程序日志，方便从日志中定位程序运行过程中出现的问题。这里的日志除了日志内容本身之外，还应该包括文件名/行号，时间戳，线程/协程号，模块名称，日志级别等额外信息，甚至在打印致命的日志时，还应该附加程序的栈回溯信息，以便于分析和排查问题。   
&emsp;从设计上看，一个完整的日志模块应该具备以下功能：  
>1.区分不同的级别，比如常的DEBUG/INFO/WARN/ERROR/FATAL等级别。日志模块可以通过指定级别实现只输出某个级别以上的日志，这样可以灵活开关一些不重要的日志输出，比如程序在调试阶段可以设置一个较低的级别，以便看到更多的调度日志信息，程序发布之后可以设置一个较高的级别，以减少日志输出，提高性能  

>2.区分不同的输出地。不同的日志可以输出到不同的位置，比如可以输出到标准输出，输出到文件，输出到syslog，输出到网络上的日志服务器等，甚至同一条日志可以同时输出到多个输出地。  

>3.区分不同的类别。日志可以分类并命名，一个程序的各个模块可以使用不同的名称来输出日志，这样可以很方便地判断出当前日志是哪个程序模块输出的。  

>4.日志格式可灵活配置。可以按需指定每条日志是否包含文件名/行号、时间戳、线程/协程号、日志级别、启动时间等内容。  

>5.可通过配置文件的方式配置以上功能。

## 关键实现  
### LogEvent  
日志事件，用于记录日志现场，具体包括以下内容：
```
日志内容
日志器名称
日志级别
文件名，对应__FILE__宏
行号，对应__LINE__宏
程序运行时间，通过sylar::GetElapsedMS()获取
线程ID
协程ID
UTC时间戳，对应time(0)
线程名称
```
日志事件的构造需要通过宏来简化，否则，每次生成一个日志事件时都要对上面这些内容进行赋值，够麻烦的。
--- 
### 工具宏
原作者sylar定义了一系列工具宏用于简化编码以及实现流式风格的日志输出和格式化风格的日志输出，下面以流式风格的实现为例分析一下宏的设计：
```cpp
//使用流式方式将日志级别level的日志写入到logger
//构造一个LogEventWrap对象，包裹包含日志器和日志事件，在对象析构时调用日志器写日志事件
#define SYLAR_LOG_LEVEL(logger , level) \
    if(level <= logger->getLevel()) \
        sylar::LogEventWrap(logger, sylar::LogEvent::ptr(new sylar::LogEvent(logger->getName(), \
            level, __FILE__, __LINE__, sylar::GetElapsedMS() - logger->getCreateTime(), \
            sylar::GetThreadId(), sylar::GetFiberId(), time(0), sylar::GetThreadName()))).getLogEvent()->getSS()
 
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)
 
#define SYLAR_LOG_ALERT(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ALERT)
 
#define SYLAR_LOG_CRIT(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::CRIT)
 
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
 
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
 
#define SYLAR_LOG_NOTICE(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::NOTICE)
 
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
 
#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
```
假如调用语句如下：
```cpp
sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
SYLAR_LOG_INFO(g_logger) << "info msg";
```
*那么，宏展开如下：
```cpp
sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
if(sylar::LogLevel::INFO >= g_logger->getLevel)
    sylar::LogEventWrap(logger, sylar::LogEvent::ptr(new sylar::LogEvent(logger->getName(),
    level, __FILE__, __LINE__, sylar::GetElapsedMS() - logger->getCreateTime(),
    sylar::GetThreadId(), sylar::GetFiberId(), time(0), sylar::GetThreadName()))).getLogEvent()->getSS() << "info msg";
```
&emsp;这里要实现通过g_logger打印一条INFO级别的消息。那么，首先判断INFO级别是否高于g_logger本身的日志级别。  
&emsp;如果不高于，那if语句执行不到，这条日志也不会打印，否则，临时构造一个LogEventWrap对象，传入日志器g_logger，以及现场构造的日志事件。  
&emsp;通过LogEventWrap的getLogEvent()方法拿到日志事件，再用日志事件的流式日志消息成员输出日志消息。  
&emsp;***由于LogEventWrap是在if语句内部构建的，一旦if语句执行结束，LogEventWrap对象就会析构，日志事件也就会被g_logger进行输出，这个设计可以说是非常巧妙。***
## 待完善部分
>sylar日志模块已经实现了一个完整的日志框架，并且配合后面的配置模块，可用性很高，待补充与完善的地方主要存在于LogAppender  
可以考虑添加几种LogAppender:
```
1. Rolling File Appender，循环覆盖写文件
2. Rolling Memory Appender，循环覆盖写内存缓冲区
3. 支持日志文件按大小分片或是按日期分片
4. 支持网络日志服务器，比如syslog
```