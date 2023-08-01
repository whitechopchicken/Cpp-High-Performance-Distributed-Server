#include <iostream>
#include <memory>
using namespace std;

class A {
public:
    static std::shared_ptr<A> getA() {
        if (!m_A) {
            m_A = std::shared_ptr<A>(new A("singleton project"));
        }
        return m_A;
    }

    void show() {
        std::cout << m_name << std::endl;
    }

private:
    A(const string name) : m_name(name) {}
    A();                    // 默认构造私有
    A(const A&);            // 拷贝构造也私有
    A& operator=(const A&); // 赋值构造也私有
private:
    static std::shared_ptr<A> m_A; // 定义一个static shared_ptr 实例, 类外实例化
    string m_name;
};
std::shared_ptr<A> A::m_A = nullptr;
