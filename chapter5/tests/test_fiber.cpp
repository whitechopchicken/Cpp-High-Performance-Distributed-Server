#include "../src/yy.h"
/**/

yy::Logger::ptr g_logger = YY_LOG_ROOT();

void run_in_fiber() {
    YY_LOG_INFO(g_logger) << "run_in_fiber begin";
    yy::Fiber::YieldToHold();
    YY_LOG_INFO(g_logger) << "run_in_fiber end";
    yy::Fiber::YieldToHold();
}

void test_fiber() {
    YY_LOG_INFO(g_logger) << "main begin -1";
    {
        yy::Fiber::GetThis();
        YY_LOG_INFO(g_logger) << "main begin";
        yy::Fiber::ptr fiber(new yy::Fiber(run_in_fiber));
        fiber->swapIn();
        YY_LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        YY_LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    YY_LOG_INFO(g_logger) << "main after end2";
}

int main() {
    yy::Thread::SetName("main");
    std::vector<yy::Thread::ptr> thrs;
    for(int i=0;i<3;i++) {
        thrs.push_back(yy::Thread::ptr(
            new yy::Thread(test_fiber, "name_" + std::to_string(i))));
    }
    for(auto i : thrs) {
        i->join();
    }
    return 0;
}