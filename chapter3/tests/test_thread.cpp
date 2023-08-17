#include"../src/yy.h"
#include<unistd.h>

yy::Logger::ptr g_logger1 = YY_GET_ROOT();

int count = 0;
//yy::RWMutex s_mutex;
yy::Mutex s_mutex;

void func1() {
    YY_LOG_INFO(g_logger1) << " name:" << yy::Thread::GetName() 
                              << " this.name: " <<yy::Thread::GetThis()->getName()
                              << " this.id: " << yy::Thread::GetThis()->getId();
    
    for(int i=0;i<10;i++){
        //yy::RWMutex::WriteLock lock(s_mutex);
        yy::Mutex::Lock lock(s_mutex);
        count++;
    }
}

void func2() {
    while(true) {
        YY_LOG_INFO(g_logger1) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
    
}
void func3() {
    while(true) {
        YY_LOG_INFO(g_logger1) << "================================================================";
    }
}

int main(){
    YY_LOG_INFO(g_logger1) << "thread test brgin";
    YAML::Node root = YAML::LoadFile("/home/yyshinidaye/yy/chapter3/bin/conf/log2.yaml");   // 
    yy::Config::LoadFromYaml(root);
    std::vector<yy::Thread::ptr> thrs;
    for(int i=0;i<2;i++){
        yy::Thread::ptr thr(new yy::Thread(&func2, "name_"+ std::to_string(i * 2)));
        //yy::Thread::ptr thr2(new yy::Thread(&func3, "name_"+ std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        //thrs.push_back(thr2);
    }
    
    for(size_t i=0;i<thrs.size();i++){
        thrs[i]->join();
    }
    YY_LOG_INFO(g_logger1) << "thread test end";
    YY_LOG_INFO(g_logger1) << "count = " << count;

    
    return 0;
}