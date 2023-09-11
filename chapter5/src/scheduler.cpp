#include "scheduler.h"
#include "log.h"
#include "macro.h"

namespace yy {
static yy::Logger::ptr g_logger  =  YY_LOG_NAME("system");   //自己函数库的功能都放到system里，便于业务代码&系统代码的日志分离//在util.cpp有一个同名的 g_logger,所以加static

//线程局部变量声明一个协程调度器指针
static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) :m_name(name){
    YY_ASSERT(threads > 0);

    if(use_caller) {
        yy::Fiber::GetThis();//如果该线程没有协程，那就给他初始化个主协程
        --threads;

        YY_ASSERT(GetThis() == nullptr);
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this))); //bind() 把函数参数和函数绑定，这样可以调用该函数对象
        yy::Thread::SetName(m_name);
        
        t_fiber = m_rootFiber.get();    //在线程里声明一个调度器，再把当前线程放入调度器，它的主协程不再是那个线程的主协程了，而是在执行run() 的主协程
        m_rootThread = yy::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    }else {
        m_rootThread = -1;
    }
    m_threadCount = threads;
}
//构造函数：其实只是创建了一个协程，执行了一个 run() ,但这个协程还没有真正执行起来
Scheduler::~Scheduler(){
    YY_ASSERT(m_stopping);
    if(GetThis() == this) {
        t_scheduler = nullptr;
    }
}

Scheduler* Scheduler::GetThis() {
    return t_scheduler;
}

Fiber* Scheduler::GetMainFiber() {
    return t_fiber;
}

//核心方法！
void Scheduler::start() {
    MutexType::Lock lock(m_mutex);
    if(!m_stopping) {
        return ;    //还没启动
    }
    m_stopping = false;
    YY_ASSERT(m_threads.empty());   //现在线程池肯定是空的

    m_threads.resize(m_threadCount);    //这就是(线？)协程里放信号量Semophore，等它回来时，确保协程函数跑起来，把它的ID初始好，那在外面就能拿到真正的线程id
    for(size_t i = 0 ; i < m_threadCount; i++) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" +std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }//线程池也启动好了
    lock.unlock();

    // if(m_rootFiber) {
    //     //m_rootFiber->swapIn();  
    //     m_rootFiber->call();
    //     YY_LOG_INFO(g_logger) << "call out";
    // }
}
// 协程没事做的时候，就把他stop
void Scheduler::stop() {
    m_autoStop = true;
    if(m_rootFiber && m_threadCount == 0 
        && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)) {
            YY_LOG_INFO(g_logger) <<this << "stopped";
            m_stopping = true;

            if(m_stopping) {
                return; //use_caller 并且只有一个协程的情况，就从这里返回;否则，下面的代码就是每个协程tickle一下，然后再主协程返回
            }
        }
    //bool exit_on_this_fiber = false;
    if(m_rootThread != -1) {    //说明是use_caller 的线程
        YY_ASSERT(GetThis() == this);
    }else {
        YY_ASSERT(GetThis() != this);
    }

    m_stopping = true;
    for(size_t i = 0; i<m_threadCount; i++) {
        tickle();
    }
    
    if(m_rootFiber) {
        tickle();   //有多少个run，就运行多少个tickle
    }
    if(stopping()) {
        return ;
    }
}

void Scheduler::setThis() {
    t_scheduler = this;
}

//写这个协程到底在做什么事情
void Scheduler::run() {
    YY_LOG_INFO(g_logger) << "run" ;    //用日志来 测试有没有进入run()
    setThis();
    if((int)yy::GetFiberId() != m_rootThread) {        //等于的情况在上面已经初始化过了
        t_fiber = Fiber::GetThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;

    FiberAndThread ft;
    while(true) {
        ft.reset();
        bool tickle_me = false;
        {
            MutexType::Lock lock(m_mutex);
            auto it = m_fibers.begin(); //对应的协程消息队列
            while(it != m_fibers.end()) {
                if(it->thread != -1 && it->thread != yy::GetThreadId()) {
                    //当前执行的run 不等于这个线程
                    ++it;   //它不处理，但是要通知别的线程去处理
                    tickle_me = true;
                    continue;
                }
                YY_ASSERT(it->fiber || it->cb); //fiber或者回调函数 至少有一个
                if(it->fiber && it->fiber->getState() == Fiber::EXEC) { //正在执行状态
                    ++it;
                    continue;
                }

                ft = *it;
                m_fibers.erase(it);
            }
        }
        //上面取出了一个需要执行的消息
        if(tickle_me) {
            tickle();   //通知其他线(协？)程
        }

        if(ft.fiber && (ft.fiber->getState() != Fiber::TERM || ft.fiber->getState() != Fiber::EXCEPT)) {
            ++m_activeThreadCount;
            ft.fiber->swapIn(); //swapIn唤醒，让它去执行
            --m_activeThreadCount;

            if(ft.fiber->getState() == Fiber::READY) {
                schedule(ft.fiber);
            }else if(ft.fiber->getState() != Fiber::TERM 
                && ft.fiber->getState() != Fiber::EXCEPT) {
                    ft.fiber->m_state = Fiber::HOLD;        //m_state是fiber的私有成员，在scheduler的函数里不能直接访问吧？
                }
                ft.reset();
        }else if(ft.cb) {
            if(cb_fiber) {
                cb_fiber->reset(ft.cb);    // 这里似乎不能直接加 & ？
            }else {
                cb_fiber.reset(new Fiber(ft.cb));
            }
            ft.reset();   
            ++m_activeThreadCount;   //直接换成atomic ，就避免线程安全问题
            cb_fiber->swapIn();
            --m_activeThreadCount;
            if(cb_fiber->getState() == Fiber::READY) {
                schedule(cb_fiber);
                cb_fiber.reset();
            }else if(cb_fiber->getState() == Fiber::EXCEPT || cb_fiber->getState() == Fiber::TERM) {
                cb_fiber->reset(nullptr);
            }else {//if(cb_fiber->gerState() != Fiber::TERM) {
                cb_fiber->m_state = Fiber::HOLD;
                cb_fiber.reset();
            }
        }else {
            if(idle_fiber->getState() == Fiber::TERM) {
                YY_LOG_INFO(g_logger) << "idle fiber term";
                break;  //如果。。。退出while，否则 m_idle = 
            }

            m_idelThreadCount++;
            idle_fiber->swapIn();
            --m_idelThreadCount;
            if(idle_fiber->getState() != Fiber::TERM || idle_fiber->getState() != Fiber::EXCEPT) {
                idle_fiber->m_state = Fiber::HOLD;      //其他类的函数直接访问私有？？
            }
        }
    }//退出while
}

void  Scheduler::tickle() {
    YY_LOG_INFO(g_logger) << "tickle";
}
bool Scheduler::stopping() {
    //返回，指示这个任务是否完成
    MutexType::Lock lock(m_mutex);
    return m_autoStop && m_stopping && m_fibers.empty() && m_activeThreadCount ==0 ;
}
void Scheduler::idle() {
    YY_LOG_INFO(g_logger) <<"idle";
}


}

 