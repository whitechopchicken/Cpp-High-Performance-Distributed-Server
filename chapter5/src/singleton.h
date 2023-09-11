#pragma once

#include <memory>

namespace yy{

template<typename T>
class Singleton{
public:
    static T* GetInstance(){
        static T value;
        return &value;
    }
private:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

template<typename T>
class SingletonPtr{
public:
    typedef std::shared_ptr<T> ptr;
    static ptr GetInstance(){
        static ptr value(new T);
        return value;
    }

private:
    SingletonPtr() = default;
    SingletonPtr(const SingletonPtr&) = delete;
    SingletonPtr& operator=(const SingletonPtr&) = delete;
};



}