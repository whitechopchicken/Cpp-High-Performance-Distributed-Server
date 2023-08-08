#include "thread.h"
#include "log.h"

namespace{

static thread_local std::string t_thread_name = "UNKNOW";

const std::string& yy::Thread::GetName(){ return t_thread_name;}    //返回一个默认值 “UNKNOW”



}