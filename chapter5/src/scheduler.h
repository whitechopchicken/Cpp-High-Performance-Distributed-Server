#pragma once

#include <memory>
#include "fiber.h"
#include "thread.h"
#include <vector>
#include <list>
#include <map>

namespace yy {

class Scheduler {   //包含线程池
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();

    const std::string& getName() const {return m_name;}

    static Scheduler* GetThis();
    static Fiber* GetMainFiber();

    void start();
    void stop();
    void run();

    template<class FiberOrCb> 
    //无锁的方法
    void schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        if(need_tickle) {
            tickle();
        }

    }
    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end) { 
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);  //锁一次，把所有放进去，确保这组连续的任务一定在连续的消息队列，按照连续顺序执行
            while(begin != end) {
                need_tickle = scheduleNoLock(&*begin) || need_tickle;   //取地址，会把里面的东西swap掉 
            }
        }
        if(need_tickle) {
            tickle();
        }
    }
protected:
    virtual void  tickle();
    virtual bool stopping();
    virtual void idle();//虽然没任务，仍然把cpu占住 == 与epoll关联

    void setThis();
private:    //作用：放进去了，如果空，返回true，说明以前没有任务，所有线程都陷入wait，当我放进去一个，就唤醒线程，通知有任务了，从协程队列里取出协程
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if(ft.fiber|| ft.cb) {  //要么是协程，要么是functional
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }
private:
    //支持什么样的执行参数：协程和functional
    struct FiberAndThread {
        Fiber::ptr fiber;
        std::function<void()>cb;
        int thread;

        //两种传参方式，如果给的是指针，那就去swap()
        FiberAndThread(Fiber::ptr f, int thr): fiber(f), thread(thr) {}
        FiberAndThread(Fiber::ptr* f, int thr):thread(thr) {
            fiber.swap(*f); //为什么：传协程智指的智指 ？==> 说是避免可能的 “释放问题”  
        }
        FiberAndThread(std::function<void()>f , int thr):cb(f), thread(thr) {}
        FiberAndThread(std::function<void()> * f, int thr): thread(thr) {
            cb.swap(*f);        
        }
        //为什么写默认构造？：stl里的知识。 否则分配出的对象无法初始化
        FiberAndThread() :thread(-1){}//-1表示不执行任何线程
        
        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

private:
    MutexType m_mutex;
    std::vector<Thread::ptr> m_threads;
    std::list<FiberAndThread> m_fibers;
    std::map<int, std::list<FiberAndThread> > m_thrFibers;
    Fiber::ptr m_rootFiber; //主协程
    std::string m_name;

protected:
    std::vector<int> m_threadIds; //存线程id
    size_t m_threadCount = 0;
    std::atomic<size_t> m_activeThreadCount = {0};
    std::atomic<size_t> m_idelThreadCount = {0};
    bool m_stopping = true;
    bool m_autoStop = false;
    int m_rootThread = 0;   //主线程id

};

}