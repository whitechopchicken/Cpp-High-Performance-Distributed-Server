#pragma once

#include<memory>
#include<ucontext.h>
#include<functional>
#include"thread.h"

namespace yy {

class Scheduler;

class Fiber : public std::enable_shared_from_this<Fiber> {
friend class Scheduler; //这样就能在 scheduler::run() 代码里直接访问 fiber->m_state 私有成员了
public:
    typedef std::shared_ptr<Fiber> ptr;
    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };

private:
    Fiber();
public:
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();
    //重置协程函数，并重置状态
    //INIT TERM
    void reset(std::function<void()> cb);
    //切换到当前协程执行
    void swapIn();
    //切换到后台执行
    void swapOut();

    void call();//直接强行把当前协程置换成目标执行协程，与swapIn有区别

    uint64_t getId() const { return m_id;}      //返回协程id

    State getState() const {return m_state;}    //返回协程状态

public:
    //设置当前协程
    static void SetThis(Fiber* f);
    //返回当前协程
    static Fiber::ptr GetThis();
    //协程切换到后台，并且设置为Ready状态
    static void YieldToReady();
    //协程切换到后台，并且设置为Hold状态
    static void YieldToHold();
    //总协程数
    static uint64_t TotalFibers();

    static void MainFunc();

    static uint64_t GetFiberId();   

private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;   //协程运行栈大小
    State m_state = INIT;       //协程状态 。私有的，又没有set接口，为啥后面直接访问？

    ucontext_t m_ctx;           //协程上下文
    void* m_stack = nullptr;    //协程运行栈指针

    std::function<void()> m_cb; //协程运行函数

};


}

