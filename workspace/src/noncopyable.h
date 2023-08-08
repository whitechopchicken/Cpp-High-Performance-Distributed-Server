//经典技巧
//定义一个辅助类 noncopyable，把这个类的拷贝构造、赋值函数 写成禁用模式
//当我们定义一些其他类，并且不想让这些类拥有复制、拷贝等功能时，就让这些类继承 noncopyable

#pragma once

namespace yy{

class Noncopyable{

public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    /*拷贝构造函数，设为禁用*/
    Noncopyable(const Noncopyable&) = delete;

    /*赋值构造函数， 设为禁用*/
    Noncopyable& operator=(const Noncopyable&) = delete;

};

}