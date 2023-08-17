#include "../src/yy.h"
#include<assert.h>

yy::Logger::ptr g_logger = YY_GET_ROOT();

void test_assert() {
    YY_LOG_INFO(g_logger) << yy::BacktraceToString(10, 2, "      ");
    YY_ASSERT2(0 == 1, "abchdj xx");
}


int main() {
    test_assert();
    return 0;
}