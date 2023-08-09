#pragma once 

#include<pthread.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<unistd.h>
#include<stdint.h>


namespace yy{

pid_t GetThreadId();    //在util.cpp 中实现
uint32_t GetFiberId() {return 0;}

}