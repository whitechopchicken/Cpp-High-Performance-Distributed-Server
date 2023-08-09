#include "util.h"

namespace yy{
    pid_t GetThreadId(){
        return syscall(SYS_gettid);
    }
}