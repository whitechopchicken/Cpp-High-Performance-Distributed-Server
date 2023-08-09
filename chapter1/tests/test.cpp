#include "../src/log.h"
#include "../src/util.h"

int main(){


    auto l = yy::loggerMgr::getInstance()->getLogger("xx");
    YY_LOG_INFO(l) << "xxxxx";

    return 0;


}