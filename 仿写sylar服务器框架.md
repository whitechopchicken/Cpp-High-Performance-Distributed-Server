### 当前进度，协程模块，chapter5    
## 开发环境：
	Ubuntu 
	gcc 
	cmake
## 项目路径
bin		-- 二进制文件
build	-- 中间文件路径
cmake	-- cmake函数文件夹
CMakeLists.txt	--cmake的定义文件
lib		-- 库的输出路径
Makefile		
src		-- 源代码路径
tests	-- 测试代码

## 日志系统
	仿照log4j 
		Logger类(定义日志类别
			|
			|-------Formatter(日志格式
			|
		Appender类(日志输出的地方
		
## 配置系统
Config -->Yaml（底层，相较于json 更简单
>Yaml 来文件配置，通过它来加载&修改配置

#### 配置系统作用： 
	项目配置参数时，方便修改. 可以通过写配置的方式，来配置我们要的日志(格式、级别、目的地 。。。)
			1.性能优化和资源管理
			2.安全性和访问控制
			3.可扩展性和负载均衡
			4.错误处理和日志记录
			5.数据库连接和数据存储设置
			6.第三方服务集成：许多服务器应用程序需要与第三方服务进行集成，如支付网关、通知服务等。配置系统可以用于设置这些集成，包括API密钥、端点URL等。。。

>步骤： 首先定义一个基类 ， 然后用模板类继承

>P10 讲 如何加载Yaml，基本的用法，遍历Yaml

>P11 把Yaml 和 config 整合在一起

>p12 在原来config类型上做了些改进，  
```
template<T, FormStr, ToStr> 
class ConfigVar;

template<F, T>
LexicalCast;

//容器偏特化，写到了支持vector ---偏特化，就是template<1,2>里面的1 换成具体的某个type ，代码里对typename的使用有点强。 lexicalCast 主要用与不同类型之间的转化，比如 int转string 等。
```
>p13 对更对stl容器的支持
 ```
 主要还是根据p12 的方式，对更多的stl 类型转化进行了支持，使用偏特化模板方式
 支持 map  list unordered_map unordered_set 
 在常规的配置解析中，已经可以完成大部分工作。
 // map/unordered_set 支持 key = std::string
 // Config::Lookup(key) , key 相同
 还有遗漏: 类型不同的，不会有报错。这个需要处理一下
 ```
>p14  自定义类型，需要实现yy::LexicalCast 偏特化。 实现后，就可以支持Config解析自定义类型和常规stl容器一起使用，不管嵌套多深都可以解析出来(因为模板使用了递归解析).

>p15 当配置发生变更时，让它触发一个事件-->可以通过事件来捕捉这个变更(每个人业务不同。。。)
```
配置的事件机制： 
	当一个配置项发生修改时，可以反向通知代码回调 
	在线程出现之前，算是完整了
```

>p16 日志系统整合01   
代码部分： bin/conf.log.yml&emsp;tests/test_config.cc注意绝对路径与源代码不同&emsp; logger.h(主要是Manager添加了friend)&emsp;sylar/config.h
#### 以下是我们想要的日志输出格式
```yaml
logs:
	- name: root
	  level: (debug, info, warn, error, fatal)
	  formatter: '%d%T%p%t%m%n'
	  appender: 
	  	- type: (StroutLogAppender, FileLogAppender)
		  level: (debug,...)
		  file: /logs/xxx.log
```
```
29:10 知识点：在main() 之前会执行什么代码
做一个全局的对象，它会在main() 之前构造，就可以在这里加事件  ---> config.h 
struct LogIniter{
	LogIniter(){
		g_log_defines->addListener(0xF1E231,[](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value))
	};
};
static LogIniter __log_init;
```
>p17 日志系统整合02   
30:00 有个 静态成员变量初始化不一致的bug，然后解决了。effective C++ 04 确定对象初始化  
47:00 数组应该是a[] ，这里都写成n[]了。
```cpp
//定义LogDefine  LogAppenderDefine ，偏特化 LexicalCast
//实现日志配置解析
```

>p18  日志系统整合03 &emsp; yaml映射到log的配置
代码部分：  log.c关于class LexicalCast<>偏特化&emsp;各种::toYamlString()  
test_config.cc &emsp;  log.yml  

>p19  
```cpp
//作者在这里完善Logger::addAppender() 和 delAppender() ，弹幕说可以改进~用一个flag标志就搞定了
```
>*p20 类图，总结之前的log各种类之间的关系 

## 线程模块

>p22 mutex 和 信号量semaphore  
为什么不用c++11 的std::thread? 因为它只有一种互斥量，没有读写分离的方式&emsp; 弹幕说c++17支持 std::shared_mutex  
```cpp
//弹幕“信号量感觉不如条件变量好用” == 虽然我也只学了条件变量~_~std::condition_variable  写了个 Semaphore::wait() 和 Semaphore::nority() 
//没有消息就wait()  有消息就notify() 唤醒一个线程 
```
## 协程模块 --协程可以在特定的点上暂停并保存其状态，然后在需要的时候恢复执行。  
户级别线程库可以使用 ucontext 来实现协程，这是一种轻量级的线程，可以在不同函数之间进行切换，实现一种协作式多任务。
> p26 定义协程接口，定义一些宏，方便调试 macro.h ,包含assert() 断言，判断一些有问题的地方  
> P27 Fiber的初始构造和析构函数   
> p28 t_threadFiber 这里用了智能指针的智能指针，有点奇怪~    
写了协程的一些基本用法，比如初始化、swapin 、swapout-->需要解决：协程的析构(释放)问题  
一个线程thread 可以通过 Fiber::GetThis() 来获得当前的协程,如果还没有，那就创建一个主协程  
>p29   协程切换：只能由某个子协程切换到主协程  
>以上都是在同一个线程中，进行协程切换，如何在不同线程中切换协程？==>协程调度

## IO、协程调度模块  --先看源代码，搞清楚思路
### 协程调度模块 P30~p35  
> p30 协程调度的 scheduler类的接口、架构-->后续会把epoll的功能和协程调度的子类结合起来  
> p31 实现Fiber的析构、构造、start()、stop() 等函数，，还需要把run() 写出来再修正stop()  
> p32 run():如果没有任务时，协程应该做什么  
```
run() :   
1.设置当前线程的scheduler
2.设置当前线程的run，fiber
3.协程调度循环while(true)--> 协程消息队列是否有任务？-->无任务执行，执行idle
```    
> p33 调试。18:00左右，gdb调试发现死锁，值得学习！！！ 后面一直在gdb调试 swapIn 和 call   ，这里确实讲的不太好，没理解在干嘛
> P34  调试 ，最后日志正常了，但是以idle执行完为结束，没解释。。 好难！  
use_caller 服用了已经结束的协程，然后balabala。。。   有了个基本可以跑的用例
> p35  完善协程调度的接口  
听sylar讲，害得自己调试几遍。。。  协程调度器，如果在同一个线程里面调度不同协程还好，但是需要在《多线程》里面调度《多个协程》，特殊的地方需要把当前使用的《线程》加入到《协程调度器里面》，调试这个需要很长时间。。
### IO协程调度模块 p36~p42  
> p36 支持异步IO --  底层用epoll来实现 IOManager (epoll) --> 继承 Scheduler
## Hook模块

## Socket模块

## ByteArray序列化模块

## TcpServer模块

## Stream模块

## http协议开发

## 分布式协议     ---把要做的业务放在插件中，把系统和业务分开

## 推荐系统	  ---去网上抓取一些用户浏览数据，让我们访问的时候，推荐
