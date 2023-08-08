/*
test.cpp 里面的宏需要用到 thread这个类，所以先写thread， thread需要 mutex，所以再先写mutex.h
*/
#pragma once

#include <memory>
#include <string>
#include "mutex.h"
#include "noncopyable.h"
#include <functional>
 
namespace yy{

class Thread : Noncopyable{

public:
    typedef std::shared_ptr<Thread> ptr;

    Thread(std::function<void()> cb, const std::string& name);//cb 线程执行函数， name 线程名
    ~Thread();
    pid_t getPid()const { return m_pid;}//获取线程ID
    const std::string& getName()const { return m_name;}//获取线程名---小写的get
    //等待线程执行完成
    //获取当前线程指针
    static const std::string& GetName();//获取当前线程名---与前面的不同---大写的Get
    //设置当前线程名---

private:
    static void* run(void* arg);

private:
    pid_t m_pid;//线程id
    pthread_t m_thread;//线程结构
    std::function<void()> m_cb;//线程执行函数
    std::string m_name;//线程名称
    //信号量----待加入
};





}