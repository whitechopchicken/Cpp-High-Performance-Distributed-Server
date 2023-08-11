#include "../src/log.h"
#include "../src/config.h"
#include <yaml-cpp/yaml.h>

yy::ConfigVar<int>::ptr g_int_value_config = 
    yy::Config::Lookup("system.port", (int)8080, "system port");

//同一名字，类型不可更改 Lookup中error
yy::ConfigVar<float>::ptr g_floatx_value_config = 
    yy::Config::Lookup("system.port", (float)8080, "system port");

yy::ConfigVar<float>::ptr g_float_value_config = 
    yy::Config::Lookup("system.value", (float)10.21f, "system value");

yy::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config = 
    yy::Config::Lookup("system.int_vec", std::vector<int>{1,2,3}, "system int vec");

yy::ConfigVar<std::list<int>>::ptr g_int_list_value_config = 
    yy::Config::Lookup("system.int_list", std::list<int>{1,2}, "system int list");

yy::ConfigVar<std::set<int> >::ptr g_int_set_value_config = 
    yy::Config::Lookup("system.int_set", std::set<int>{11, 22}, "system int set");

yy::ConfigVar<std::unordered_set<int> >::ptr g_int_unset_value_config = 
    yy::Config::Lookup("system.int_unset", std::unordered_set<int>{2, 1}, "system int unset");

yy::ConfigVar<std::map<std::string, int> >::ptr g_str_int_map_value_config = 
    yy::Config::Lookup("system.str_int_map", std::map<std::string, int>{{"k", 21}}, "system str_int map");

yy::ConfigVar<std::unordered_map<std::string, int> >::ptr g_str_int_unmap_value_config = 
    yy::Config::Lookup("system.str_int_unmap", std::unordered_map<std::string, int>{{"k", 21}, {"k2", 10}}, "system str_int unmap");
//递归遍历YAML节点，打印到日志中
void print_yaml(const YAML::Node& node, int level){
    if(node.IsScalar()){
        YY_LOG_INFO(YY_GET_ROOT()) << std::string(level * 4, ' ')
            <<node.Scalar() << " - " << node.Type() << " - " << level;
    }
    else if(node.IsNull()){
        YY_LOG_INFO(YY_GET_ROOT()) << std::string(level * 4, ' ')
            << "NULL - " << node.Type() <<level;
    }
    else if(node.IsMap()){
        for(auto it = node.begin();
                it != node.end();
                it++){
            YY_LOG_INFO(YY_GET_ROOT()) << std::string(level * 4, ' ')
                << it->first << " - " << it->second.Type() << " - "<<level;
            print_yaml(it->second, level+1);    //递归配置？
        }
    }
    else if(node.IsSequence()){
        for(size_t i = 0; i<node.size(); i++){
            YY_LOG_INFO(YY_GET_ROOT()) << std::string(level * 4, ' ')
            << i << " - " << node[i].Type() << " - " <<level;
            print_yaml(node[i], level+1);       //递归配置
        }
    }
}

void test_yaml(){
    YAML::Node root = YAML::LoadFile("/home/yyshinidaye/yy/chapter2/bin/conf/log.yaml");

    print_yaml(root, 0);

    //YY_LOG_INFO(YY_GET_ROOT()) << root ;
}

void test_config() {
    YY_LOG_INFO(YY_GET_ROOT()) << "before:" << g_int_value_config->getValue();
    YY_LOG_INFO(YY_GET_ROOT()) << "before:" << g_float_value_config->getValue();
#define XX(g_var, name, prefix) \
    {   \
        auto& v = g_var->getValue();    \
        for(auto& i : v){   \
            YY_LOG_INFO(YY_GET_ROOT()) << #prefix " " #name " : " << i;    \
        }   \
        YY_LOG_INFO(YY_GET_ROOT()) << #prefix " " #name " yaml: " << g_var->toString();    \
    }

#define XX_M(g_var, name, prefix) \
    {   \
        auto& v = g_var->getValue();    \
        for(auto& it : v){   \
            YY_LOG_INFO(YY_GET_ROOT()) << #prefix " " #name " : (" << it.first << " - " << it.second << ")";    \
        }   \
        YY_LOG_INFO(YY_GET_ROOT()) << #prefix " " #name " yaml: " << g_var->toString();    \
    }
    XX(g_int_vec_value_config, int_vec, before);
    XX(g_int_list_value_config, int_list, before);
    XX(g_int_set_value_config, int_set, before);
    XX(g_int_unset_value_config, int_unset, before);

    XX_M(g_str_int_map_value_config, str_int_map, before);
    XX_M(g_str_int_unmap_value_config, str_int_unmap, before);


    YAML::Node root = YAML::LoadFile("/home/yyshinidaye/yy/chapter2/bin/conf/log.yaml");
    yy::Config::LoadFromYaml(root);

    YY_LOG_INFO(YY_GET_ROOT()) << "after:" << g_int_value_config->getValue();
    YY_LOG_INFO(YY_GET_ROOT()) << "after:" << g_float_value_config->getValue();

    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, int_list, after);
    XX(g_int_set_value_config, int_set, after);
    XX(g_int_unset_value_config, int_unset, after);

    XX_M(g_str_int_map_value_config, str_int_map, after);
    XX_M(g_str_int_unmap_value_config, str_int_unmap, after);

}
//为什么把数据放在public？
class Person{
public:
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const {
        std::stringstream ss;
        ss  << "[Person name = " <<m_name
            <<" age = " << m_age
            <<" sex = " << m_sex
            <<" ]";
        return ss.str();
    }

    bool operator== (const Person& other) const {
        return this->m_name == other.m_name
                && this->m_age == other.m_age
                && this->m_sex == other.m_sex;
    }
};

yy::ConfigVar<Person>::ptr g_person_config = 
    yy::Config::Lookup("class.person", Person(), "class person");

yy::ConfigVar<std::map<std::string, Person> >::ptr g_person_map_config = 
    yy::Config::Lookup("class.map", std::map<std::string, Person>{}, "class person map");

yy::ConfigVar<std::map<std::string, std::vector<Person> > >::ptr g_person_vec_map_config = 
    yy::Config::Lookup("class.vecmap", std::map<std::string, std::vector<Person> >{}, "class person vec map");

namespace yy{
    //偏特化 转换模板LexicalCast<T1,T2>
template<>
class LexicalCast<std::string, Person>{
public:
    Person operator() (const std::string& v){
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class LexicalCast<Person, std::string> {
public:
    std::string operator() (const Person& p) {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}


void test_class(){
    YY_LOG_INFO(YY_GET_ROOT()) << "before:" << g_person_config->getValue().toString() << " - " << g_person_config->toString();
#define XX_PM(g_var, prefix)    \
    {   \
        auto x = g_var->getValue();   \
        for(auto& i : x){   \
            YY_LOG_INFO(YY_GET_ROOT()) << prefix << " : " << i.first << " - " << i.second.toString(); \
        }   \
        YY_LOG_INFO(YY_GET_ROOT()) << prefix << " : size() = " << x.size();  \
    }
    XX_PM(g_person_map_config, "class.map before");

#define XXPVM(g_var, prefix)    \
    {   \
        auto x = g_var->getValue();   \
        for(auto& i:x){ \
            YY_LOG_INFO(YY_GET_ROOT()) << prefix << " : " << i.first << " = [ ";   \
            for(auto& j:i.second){  \
                YY_LOG_INFO(YY_GET_ROOT()) << j.toString() << ", ";   \
            }   \
            YY_LOG_INFO(YY_GET_ROOT()) << " ] "; \
        }   \
        YY_LOG_INFO(YY_GET_ROOT()) << prefix << " : size = " << x.size();     \
    }

    XXPVM(g_person_vec_map_config, "class.vecmap before");

    //注册更改的回调函数
    g_person_config->setListener(10, [](const Person& old_val, const Person& new_val){
        YY_LOG_INFO(YY_GET_ROOT()) << "old_val = " << old_val.toString() 
                << " new_val = " << new_val.toString();
    });

    YAML::Node root = YAML::LoadFile("/home/ubuntu/StudyCpp/yy/bin/conf/log.yaml");
    yy::Config::LoadFromYaml(root);

    YY_LOG_INFO(YY_GET_ROOT()) << "after:" << g_person_config->getValue().toString() << " - " << g_person_config->toString();
    XX_PM(g_person_map_config, "class.map after");
    XXPVM(g_person_vec_map_config, "class.vecmap after");
}

int main(){
    //test_config();
    test_class();
    return 0;
}