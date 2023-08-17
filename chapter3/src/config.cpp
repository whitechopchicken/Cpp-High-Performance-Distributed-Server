#include "config.h"


namespace yy{
ConfigVarBase::ptr Config::LookupBase(const std::string& name){
    RWMutexType::ReadLock lock(GetMutex());
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second;
}
static void ListAllMember(const std::string& prefix,
                          const YAML::Node& node,
                          std::list<std::pair<std::string, const YAML::Node> >& output){
    if(prefix.find_first_not_of("abcdefghijklmnoporstuvwxyz._012345678") != std::string::npos){
        YY_LOG_ERROR(YY_GET_ROOT()) << "Config invaild name: " << prefix << " : " <<node;
        return ;
    }
    output.push_back(std::make_pair(prefix, node));
    if(node.IsMap()){
        for(auto it = node.begin();
                it != node.end(); ++it){
            //YY_LOG_INFO(YY_GET_ROOT()) << it->first.Scalar() << " - " << it->second;
            ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
        }
    }
}

void Config::LoadFromYaml(const YAML::Node& root){
    std::list<std::pair<std::string, const YAML::Node> > all_nodes;
    ListAllMember("", root, all_nodes);
    /*
    YY_LOG_INFO(YY_GET_ROOT()) << "all_nodes: ";
    for(auto& x:all_nodes){
        YY_LOG_INFO(YY_GET_ROOT()) << x.first << " - " << x.second;
    }
    
    for(auto x : Config::s_datas){
        YY_LOG_INFO(YY_GET_ROOT()) << x.first << " - " << x.second;
    }
    */
    for(auto& i : all_nodes){
        std::string key = i.first;
        if(key.empty()){
            continue;
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookupBase(key);
        if(var) {
            if(i.second.IsScalar()){
                var->fromString(i.second.Scalar());
            }
            else{
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb){
    RWMutexType::ReadLock lock(GetMutex());
    ConfigVarMap& m = GetDatas();
    for(auto it = m.begin();it != m.end(); it++) {
        cb(it->second);
    }
}
}