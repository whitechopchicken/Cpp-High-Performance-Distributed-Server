#pragma once


#include<pthread.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<unistd.h>
#include<stdint.h>


namespace yy{

pid_t GetThreadId();
uint32_t GetFiberId();

}
