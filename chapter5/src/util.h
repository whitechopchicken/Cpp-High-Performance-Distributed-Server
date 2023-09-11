#pragma once

#include<pthread.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<unistd.h>
#include<stdint.h>
#include<vector>
#include<string>


namespace yy{

pid_t GetThreadId();
uint64_t GetFiberId();

//输出函数调用栈信息  默认打印64个栈
void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);
std::string BacktraceToString(int siz = 64, int skip = 2, const std::string& prefix = "");

}

