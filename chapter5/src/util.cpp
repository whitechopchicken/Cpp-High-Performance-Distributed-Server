#include"util.h"
#include"log.h"
#include"fiber.h"
/*
 * 包含返回几层函数栈内容的函数
*/
#include <execinfo.h>


namespace yy{

static yy::Logger::ptr g_logger = YY_LOG_NAME("system");    //在scheduler.cpp中有个同名的日志变量。所以这里加static
/**/
pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

uint64_t GetFiberId() {
    return yy::Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip) {
    void** array = (void**)malloc((sizeof(void*)*size));
    size_t s = ::backtrace(array, size);
    char** strings = backtrace_symbols(array, s);
    if(strings == NULL) {
        YY_LOG_ERROR(g_logger) << "backtrace_symbols error";
        free(strings);
        free(array);
        return ;
    }
    for(size_t i = skip; i < s; ++i) {
        bt.push_back(strings[i]);
    }
    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i = 0; i < bt.size(); ++i) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}
}