#pragma once

#include<memory>
#include<ucontext.h>
#include<functional>
#include"thread.h"

namespace yy {

class Fiber : public std::enable_shared_from_this<Fiber> {
    //继承了 shared_from_this就不能在栈上面创建对象，因为它一定
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
    Fiber();    //不允许默认构造
public:
    Fiber(std::function<void()> cb, size_t stacksize = 0);  //function<> 解决了函数指针不适用的场景
    ~Fiber();
    //重置协程函数，并重置状态（因为你函数执行完了，但是栈上的内存还在，把这块内存利用起来
    //INIT TERM （要么开始执行，要么执行结束 两种状态
    void reset(std::function<void()> cb);
    //切换到当前协程执行，
    void swapIn();
    //切换到后台执行，把执行权让出来
    void swapOut();

    uint64_t getId() const { return m_id;}

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
    uint64_t m_id = 0;          //第一个协程 就是主协程，id默认为0
    uint32_t m_stacksize = 0;   //
    State m_state = INIT;

    ucontext_t m_ctx;
    void* m_stack = nullptr;

    std::function<void()> m_cb;

};


}

