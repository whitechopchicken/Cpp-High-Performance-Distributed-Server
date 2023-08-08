#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace yy
{

class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase>ptr;
    ConfigVarBase(const std::string& name, const string& description = "")
        :m_name(name)
        ,m_description(description){};
        virtual ~ConfigVarBase() {}

        const std::string& getName()const {return m_name};
        const std::string& getDescription()const {return m_description};

        virtual std::string toString() = 0;
        virtual bool fromString(const std::string& val) = 0;

protected:
    std::string m_name;
    std::string m_description;
};

template<class T>
class ConfigVar : public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar>ptr;
    ConfigVar(const string& name                //继承父类的构造函数，同时还有其他参数
            ,const string& getDescription = ""
            ,const T& default_value)
            :ConfigVarBase(m_name, m_description)
            ,m_val(default_value){
            };
    std::string toString() override{            //override 是C++11的，告诉编译器我这个函数确实是从父类的纯虚函数继承，重写

    }
    bool fromString(const std::string& val)override{
 
    }
private:
    T m_val;


};


}