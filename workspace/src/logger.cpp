#include "logger.h"

namespace yy{

Logger::Logger(const std::string& name )    //构造函数没有返回类型
    :m_name(name){
}

void Logger::debug(LogEvent::ptr event){
    debug(LogLevel::DEBUG, event);      //这里的宏 DEBUG、INFO、ERROR ...都有定义
}
void Logger::info(LogEvent::ptr event){
    debug(LogLevel::INFO, event);       //
}
void Logger::warn(LogEvent::ptr event){
    debug(LogLevel::WARN, event);
}
void Logger::error(LogEvent::ptr event){
    debug(LogLevel::ERROR, event);
}
void Logger::fatal(LogEvent::ptr event){
    debug(LogLevel::FATAL, event);
}

void Logger::addAppender(LogAppender::ptr appender){
    m_appenders.push_back(appender);
}    
void Logger::delAppender(LogAppender::ptr appender){
    for(auto it = m_appenders.begin();
        it != m_appenders.end();
        it++){
            if(*it == appender){
                m_appenders.erase(it);
                break;
            }
        }
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event){   //遍历每个appender 把它输出出来
    if(level >= m_level){
        for(auto& i : m_appenders){
            i->log(level, event);       //每个appender 都有一个log，与本log()不同！
        }
    }
}

//---------------------------------------------两种appender子类 的函数实现-------------------------------------------
void stdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event){    //appender 里面有个formatter对象，定义输出的格式
    if(level >= m_level){
        std::cout << m_formatter->format(event);         //*****这里有争议，有人说智能指针，应该用 ->  而不是 .  确实应该用 -> 
    }
}
void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level){
        m_filestream << m_formatter->format(event);     //把event按照我们要的格式输出到 m_filestream 这个类里面
    }
}
FileLogAppender::FileLogAppender(const std::string& filename)
    :m_filename(filename){
}
bool FileLogAppender::reopen(){
    if(m_filestream){               //m_filestream.is_open() 也可以 ，这里被隐式转换成一个bool
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;      //-----转换技巧，转换为bool类的方法！
}


}