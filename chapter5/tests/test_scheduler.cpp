#include "../src/yy.h"

static yy::Logger::ptr g_logger = YY_LOG_ROOT();

int main(int argc, char** argv) {
    yy::Scheduler sc;
    sc.start();     //构造函数里，把线程数减了变成0， 所以没有能够进入run() ,日志也没有显示
    sc.stop();    
    return 0;
}