#pragma once

#include <memory>

namespace yy{

//Singleton 
template<class T>
class Singleton{
public:
    static T* GetInstance(){
        static T v;
        return &v;
    }

private:
    Singleton();
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

//Singletonptr
template<class T>
class Singletonptr{
public:
    typedef std::shared_ptr<T> ptr;
    static ptr GetInstance(){
        static ptr v(new T);
        return v;
    }
private:
    Singletonptr() = default;
    Singletonptr(const Singletonptr&) = delete;
    Singletonptr& operator=(const Singletonptr&) = delete;
};



}