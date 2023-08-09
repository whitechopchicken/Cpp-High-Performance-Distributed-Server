#pragma once
#include <memory>
namespace yy{

//T 是需要单例的类


/*下面是智能指针版本*/
template<class T , class X = void , int N = 0>
class Singletonptr{
public:
    static std::shared_ptr<T> getInstance(){
        static std::shared_ptr<T> v(new T);     
        return v;
    }
//双重static ：静态局部变量和双重检查锁定来确保单例的线程安全性
private:
    Singletonptr();
    Singletonptr(const Singletonptr&) = delete;
    Singletonptr& operator=(const Singletonptr&) = delete;
};

/*下面是普通指针版本*/
template<class T, class X = void, int N = 0>
class Singleton{
public:
    static T* getInstance(){
        static T v;
        return &v;
    }

private:
    Singleton();
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};


}