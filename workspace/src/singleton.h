#pragma once

#include <memory>

namespace yy{

namespace{  //匿名空间，封装了两个函数模板 作用：？ 似乎下面的两个函数都没有用到这个模板。。

template<class T, class X, int N>
T& GetInstanceX() {         //为什么返回值是一个引用?
    static T v;
    return v;
}

template<class T, class X, int N>
std::shared_ptr<T> GetInstancePtr() {
    static std::shared_ptr<T> v(new T);
    return v;
}

}


//两个类模板

/* 单例模式封装类。 T类型  X为了创造多个实例对应的Tag  N同一个Tag创造多个实例索引 */
template <class T, class X, int N>
class Singleton {
public:
    /*返回单例裸指针*/  //作用：
    static T* GetInstance() {
        static T v;
        return &v;
    }
};
/*单例模式智能指针封装类。 T类型  X为了创造多个实例对应的Tag  N同一个Tag创造多个实例索引*/
template<class T, class X = void, int N=0>
class SingletonPtr{
public:
    /*返回单例智能指针*/
    static std::shared_ptr<T> GetInstance(){
        static std::shared_ptr<T> v(new T);
        return v;
    }
};



}