#pragma once

#include <memory>
#include <mutex>

namespace utility
{

template<typename T>

class Singleton {
    static std::shared_ptr<T> getSingleton(){
        std::lock_guard<std::mutex>lock(m_mutex);
        if(m_Singleton == nullptr){
            m_Singleton = std::shared_ptr<T>(new T())
        }
    }

private:
    Singleton();
    Singleton(const Singleton<T> & );
    ~Singleton();
    Singleton<T> & operator = (const Singleton<T>);

private:
    static std::shared_ptr<T> m_Singleton;
    static std::mutex m_mutex;
};
template<typename T>
std::shared_ptr<T>  Singleton<T>::m_Singleton  = nullptr;


std::mutex m_mutex;

}

