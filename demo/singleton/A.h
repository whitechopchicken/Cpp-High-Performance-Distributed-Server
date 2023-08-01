#include <iostream>
#include <memory>
#include <mutex>
using namespace std;

class A{

public:
    //typedef std::shared_ptr<A> A_ptr;
    static A* getA(){
        std::lock_guard<std::mutex>lock(m_mutex);
        if(m_A == NULL)
        {
            return m_A = new A("singleton project");
            //return m_A = std::shared_ptr<A>(new A("shingleton project"));
        }
        return m_A;
    }
    void show(){
        std::cout<< m_name <<std::endl;
    }
private:
    A(const string name):m_name(name){} //传参构造私有？
    A();            //默认构造私有
    A(const A&);    //拷贝构造也私有
    A& operator =  (const A &); //赋值构造也私有
    //~A();           //析构函数也私有
private:
    static A* m_A;   //定义一个static 实例, 类外实例化
    static std::mutex m_mutex;
    string m_name;
};
A* A::m_A = nullptr;
std::mutex A::m_mutex;