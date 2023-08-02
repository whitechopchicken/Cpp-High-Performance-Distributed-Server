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
LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern){
}
std::string LogFormatter::format(LogEvent::ptr event){  //作用：调用“成员类”的同名函数，来对event内容做各种解析
    std::stringstream ss;
    for(auto& i : m_items) {        //遍历成员: items数组, 每个item分别调用自己的format()
        i->format(ss, event);
    }
    return ss.str();    //返回解析后的字符串？
}
void LogFormatter::init(){
    //将输入的m_pattern(假设是字符串) 解析成一组格式模板，并将这些模板存储在 容器vec的元组中，每个元组有3个值 string、format、type
    //我这里用了c++11新特性来优化： 用枚举类表示解析状态，增加代码可读性；用std::string::substr 截取子字符串，代替逐个字符遍历
    enum ParseStatus{
        PARSE_TEXT,
        PARSE_FORMAT
    };
    std::vector<std::tuple<std::string, std::string, int> > vec;
    size_t n = 0;
    size_t m = m_pattern.size();
    std::string nstr;

    while(n<m) {                                //解析每个m_pattern的字符
        char currentChar = m_pattern[n];
        switch (currentChar)
        {
        case '%':   //有%就表示需要解析
            if((n+1) < m && m_pattern[n+1] == '%') {
                nstr.append(1, '%');
                n += 2;
            }else{
                if(!nstr.empty()){
                    vec.push_back({nstr, "", 0});
                    nstr.clear();
                }

                ParseStatus ParseStatus = PARSE_TEXT;
                size_t fmt_begin = 0;
                std::string str;
                std::string fmt;

                for(++n; n < m; ++n){
                    char nextChar = m_pattern[n];
                    if(ParseStatus == PARSE_TEXT) {
                        if(nextChar == '{' ) {
                            str = m_pattern.substr(n-1, fmt_begin-n+1 );
                            ParseStatus = PARSE_FORMAT;
                            fmt_begin = n+1;
                        }
                    }else if(ParseStatus == PARSE_FORMAT) {
                        if(nextChar == '}' ) {
                            fmt = m_pattern.substr(fmt_begin, n - fmt_begin);
                            ParseStatus = PARSE_TEXT;
                            ++n;
                            break;
                        }
                    }
                }
                if(ParseStatus ==PARSE_TEXT ) {
                    if(!str.empty()) {
                        vec.push_back({str, "", 0});
                    }
                    vec.push_back({m_pattern.substr(fmt_begin, n-fmt_begin), fmt, 1});
                }else {
                    std::cout<<"pattern parse error: "<<m_pattern <<" - " <<m_pattern.substr(n-1) << std::endl;
                    m_error = true;
                }
            }
            break;
        
        default:
            nstr.append(1, currentChar);
            ++n;
            break;
        }
    }
    if(!nstr.empty()){
        vec.push_back({nstr, "", 0});
    }
}
/**
 *  %m---消息体
 *  %p---level
 *  %r---启动后的时间
 *  。。。。
 * ==> 做LogFormatterItem 的具体实例
*/
Class MessageFormatItem : public LogFormatter::FormatItem{

};

Class LevelFormatItem : public LogFormatter::FormatItem{
public:
    void     
};



















}