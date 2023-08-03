#include "logger.h"
#include <iostream>
#include <string>

namespace yy{

//Logger 函数
Logger::Logger(const std::string& name = "root")
    :m_name(name),
    m_level(LogLevel::DEBUG){
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}
void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        auto self = shared_from_this();
        //MutexType::Lock lock(m_mutex);
        if(!m_appenders.empty()) {
            for(auto& i : m_appenders) {
                i->log(self, level, event);
            }
        }else if(m_root) {
            m_root->log(level, event);
        }
    }
}
void Logger::debug(LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);
}
void Logger::info(LogEvent::ptr event) {
    log(LogLevel::INFO, event);
}
void Logger::warn(LogEvent::ptr event) {
    log(LogLevel::WARN, event);
}
void Logger::error(LogEvent::ptr event) {
    log(LogLevel::ERROR, event);
}
void Logger::fatal(LogEvent::ptr event) {
    log(LogLevel::FATAL, event);
}
void Logger::addAppender(LogAppender::ptr appender){
    //MutexType::Lock lock(m_mutex);  //局部锁，需要在mutex头文件定义
    if(!appender->getFormatter()){
        //MutexType::Lock ll(appender->m_mutex);
        appender->m_formatter = m_formatter;
    }
    m_appenders.emplace_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender){
    //MutexType::Lock lock(m_mutex);
    for(auto it = m_appenders.begin();
            it != m_appenders.end(); ++it) {
        if(*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}
void Logger::clearAppenders(LogAppender::ptr appender){
    //MutexType::Lock lock(m_mutex);
    m_appenders.clear();
}
void Logger::setFormatter(LogFormatter::ptr val){
    //MutexType::Lock lock(m_mutex);
    m_formatter = val;

    for(auto& i : m_appenders){
        //MutexType::Lock ll(i->m_mutex);
        if(!i->m_hasFormatter) {
            i->m_formatter = m_formatter;   //设置每个appednder的格式
        }
    }
}
void Logger::setFormatter(const std::string& val){
    std::cout << "---" << val << std::endl;
    yy::LogFormatter::ptr new_val(new yy::LogFormatter(val));   //使用传入的 val 创建了一个新的 LogFormatter 对象，并用 new_val 智能指针来管理这个对象。
    if(new_val->isError()){                                     //如果当前输入格式有错
        std::cout << "Logger setFormatter name = " << m_name
                  << " value = " << val << " invalid formatter"
                  << std::endl;
        return ;
    }
    setFormatter(new_val);  //调用传LogFormatter::ptr 的设置方法！ 学到了！
}
std::string Logger::toYamlString(){
    /*待完成*/
}

//Loglevel 函数
const char* LogLevel::Tostring(LogLevel::Level level){
    switch (level)
    {
#define XX(name);
    case : LogLevel::name: \
        return #name; \
        break;
    
    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX   //---解除宏定义，避免对后面的影响
    default:
        return "UNKNOW";
    }
    return "UNKNOW";    //这句话永远不会执行，只是为了语法检查
}
LogLevel::Level Loglevel::FromString(const std::string& str){
    #define XX(level, v) \
    if(str == #v) { \
        return LogLevel::level; \
    }
    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);

    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return LogLevel::UNKNOW;
#undef XX
}

//LogEvent 函数
LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t line, uint32_t elapse
            ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
            ,const std::string& thread_name)
            :m_looger(logger),
            m_level(level),
            m_file(file),
            m_line(line),
            m_elapse(elapse),
            m_threadId(thread_id),
            m_fiberId(fiber_id),
            m_time(time),
            m_threadName(thread_name){
}
void LogEvent::format(const char* fmt, ...){
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}
void LogEvent::format(const char* fmt, va_list al){
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al); //----调用 vasprintf 函数，将格式化后的字符串保存在 buf 中，并返回格式化后的字符串长度 len
    if(len != -1) {
        m_ss << std::string(buf, len);
        free(buf);
    }
}

//LogFormatter 函数
LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern){
        init();
}

std::string Logformatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    std::stringstream ss;
    for(auto i : m_items){                      //遍历成员: items数组, 每个item分别调用自己的format()
        i->format(ss, logger, level, event);    //作用：调用“成员类”的同名函数，来对event内容做各种解析
    }
    return ss.str();                            //返回解析后的字符串？
}
std::ostream& LogFormatter::format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    for(auto& i : m_items) {
        i->format(ofs, logger, level, event);
    }
    return ofs;
}
void LogFormatter::init() {
    //vec和 m_items 用emplace_back 替代 push_back，这样可以避免临时对象的构造和拷贝，提高性能
    std::vector<std::tuple<std::string_view, std::string_view, int>> vec;
    std::string_view nstr;      //string_view 相较于 string ，它不会创建临时对象，只有一个指针和长度的信息---更快

    for (size_t i = 0; i < m_pattern.size(); ++i) {
        if (m_pattern[i] != '%') {
            nstr = m_pattern.substr(i, 1);
            continue;
        }

        if ((i + 1) < m_pattern.size()) {
            if (m_pattern[i + 1] == '%') {
                nstr = m_pattern.substr(i, 1);
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string_view str;
        std::string_view fmt;

        while (n < m_pattern.size()) {
            if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }

            if (fmt_status == 0) {
                if (m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    fmt_status = 1; // 解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if (fmt_status == 1) {
                if (m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if (n == m_pattern.size()) {
                if (str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if (fmt_status == 0) {
            if (!nstr.empty()) {
                vec.emplace_back(nstr, std::string_view(), 0);
                nstr = std::string_view();
            }
            vec.emplace_back(str, fmt, 1);
            i = n - 1;
        } else if (fmt_status == 1) {
            LOG_ERROR << "Pattern parse error: " << m_pattern << " - " << m_pattern.substr(i);
            m_error = true;
            vec.emplace_back("<<pattern_error>>", fmt, 0);
        }
    }

    if (!nstr.empty()) {
        vec.emplace_back(nstr, std::string_view(), 0);
    }
    //定义了一个静态的无序哈希表 s_format_items，用于存储日志格式的处理方式。--->原来是map，线性查找效率较低
    static std::unordered_map<std::string_view, std::function<FormatItem::ptr(std::string_view)> > s_format_items = {
#define XX(str, C) \
        {#str, [](std::string_view fmt) { return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),           // m: 消息   --->如果不用宏，则展开：{ "m", [](std::string_view fmt) { return FormatItem::ptr(new MessageFormatItem(fmt)); } }
        XX(p, LevelFormatItem),             // p: 日志级别
        XX(r, ElapseFormatItem),            // r: 累计毫秒数
        XX(c, NameFormatItem),              // c: 日志名称
        XX(t, ThreadIdFormatItem),          // t: 线程id
        XX(n, NewLineFormatItem),           // n: 换行
        XX(d, DateTimeFormatItem),          // d: 时间
        XX(f, FilenameFormatItem),          // f: 文件名
        XX(l, LineFormatItem),              // l: 行号
        XX(T, TabFormatItem),               // T: Tab
        XX(F, FiberIdFormatItem),           // F: 协程id
        XX(N, ThreadNameFormatItem),        // N: 线程名称
#undef XX
    };

    for (const auto& i : vec) {
        if (std::get<2>(i) == 0) {
            m_items.emplace_back(FormatItem::ptr(new StringFormatItem(std::string(std::get<0>(i)))));
        } else {
            auto it = s_format_items.find(std::string_view(std::get<0>(i).data(), std::get<0>(i).size()));
            if (it == s_format_items.end()) {
                std::cout << "Pattern parse error: " << m_pattern << " - %" << std::get<0>(i);
                m_error = true;
                m_items.emplace_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::string(std::get<0>(i)) + ">>")));
            } else {
                m_items.emplace_back(it->second(std::string(std::get<1>(i))));
            }
        }
    }
}

//LogAppender 函数
void LogAppender::setFormatter(LogFormatter::ptr val){
    //MutexType::Lock lock(m_mutex);
    m_formatter = val;
    if(m_formatter){
        m_hasFormatter = true;
    }else{
        m_hasFormatter = false;
    }
}
LogFormatter::ptr LogAppender::getFormatter(){
    //MutexType::Lock lock(m_mutex);
    return m_formatter;
}

/// stdout  LogAppender 函数
void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level) {
        //MutexType::Lock lock(m_mutex);
        m_formatter->format(std::cout, logger, level, event);   //format()第一个参数是os, 这里就直接输出到屏幕上
    }
}
std::string StdoutLogAppender::toYamlString() {
    /*待加入*/
}

/// file    LogAppender 函数
FileLogAppender::FileLogAppender(const std::string& filename)
    :m_filename(filename){
        reopen();       //--为什么要reopen？
}
bool FileLogAppender::reopen() {
    //MutexType::Lock lock(m_mutex);
    if(m_filestream) {  //如果文件流存在?
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;      //-----转换技巧，转换为bool类的方法！学到了！
    //return FSUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
}
void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level) {
        uint64_t now = event->getTime();
        if(now >= (m_lastTime + 3)) {
            reopen();
            m_lastTime = now;
        }
        MutexType::Lock lock(m_mutex);
        //if(!(m_filestream << m_formatter->format(logger, level, event))) {
        if(!m_formatter->format(m_filestream, logger, level, event)) {
            std::cout << "error" << std::endl;
        }
    }
}
std::string FileLogAppender::toYamlString() {
    /*待加入*/
}

//LoggerManager 函数
LoggerManager::LoggerManager() {
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

    m_loggers[m_root->m_name] = m_root;

    init();
}
Logger::ptr LoggerManager::getLogger(const std::string& name) {
    //MutexType::Lock lock(m_mutex);
    auto it = m_loggers.find(name); //查找这个logger集合里面有无叫name的logger
    if(it != m_loggers.end()) {
        return it->second;          //找到，返回logger的 ptr
    }
    /*未找到，new一个logger，并设置其 root 日志器为 m_root ，输出*/
    Logger::ptr logger(new Logger(name));
    logger->m_root = m_root;
    m_loggers[name] = logger;
    return logger;
}
void LoggerManager::init() {
    /*空*/
}
std::string LoggerManager::toYamlString() {
    /*待加入*/
}












}