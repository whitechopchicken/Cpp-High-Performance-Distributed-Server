##开发环境：
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

## 协程模块

## IO协程调度模块

## Hook模块

## Socket模块

## ByteArray序列化模块

## TcpServer模块

## Stream模块

## http协议开发

## 分布式协议     ---把要做的业务放在插件中，把系统和业务分开

## 推荐系统	  ---去网上抓取一些用户浏览数据，让我们访问的时候，推荐
