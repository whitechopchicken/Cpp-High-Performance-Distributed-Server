#include"yy.h"
#include"log.h"
namespace yy{

//获取当前线程变量，在当前线程有用的使用静态变量表示
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

static yy::Logger::ptr g_logger = YY_LOG_NAME("system");

Semaphore::Semaphore(uint32_t count){
    if(sem_init(&m_semaphore, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}
Semaphore::~Semaphore(){
    sem_destroy(&m_semaphore);
}

void Semaphore::wait(){
    if(sem_wait(&m_semaphore)) {
        throw std::logic_error("sem_wait error");
    }

}
void Semaphore::notify(){
    if(sem_post(&m_semaphore)) {
        throw std::logic_error("sem_post error");
    }
}


Thread* Thread::GetThis(){
    return t_thread;
}
const std::string& Thread::GetName(){
    return t_thread_name;
}

void Thread::SetName(const std::string& name){
    if(t_thread){
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

void* Thread::run(void* arg){
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    thread->m_id = yy::GetThreadId();
    t_thread_name = thread->m_name;
    thread->m_id = yy::GetThreadId();
    //给线程命名，最大长度16
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
    
    std::function<void()> cb;
    //防止智能指针没有被释放掉
    cb.swap(thread->m_cb);

    thread->m_semaphore.notify();
    cb();
    return 0;
}

Thread::Thread(std::function<void()> cb, const std::string& name)
            :m_cb(cb), m_name(name){
    if(name.empty()) {
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(rt) {
        YY_LOG_ERROR(g_logger) << "pthread_create thread fail, rt = " << rt
                                  << "name = " << name;
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait();
}

Thread::~Thread(){
    if(m_thread) {
        pthread_detach(m_thread);
    }
}

void Thread::join(){
    if(m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if(rt) {
            YY_LOG_ERROR(g_logger) << "pthread_join thread fail, rt = " << rt
                                  << "name = " << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

}


// #include "thread.h"
 

//  namespace yy {

// static thread_local Thread* t_thread = nullptr; 
// static thread_local
// //thread_local 关键字：用于声明线程局部存储的变量。每个线程都会有其自己的 t_thread 变量的副本，
// //互不影响。这意味着在不同的线程中，该变量的值可以是不同的。
// //static: 这是一个关键字，用于表示变量是静态的。静态变量在程序的整个生命周期内都存在，而不是在函数调用期间创建和销毁。
// Thread* Thread::GetThis();
// const std::string& GetName();

// Thread(std::function<void>() cb, const std::string& name);

//  }