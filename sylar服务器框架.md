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
Config -->Yaml
>作用： 项目配置参数时，方便修改
>步骤： 首先定义一个基类 ， 然后用模板类继承

## 协程库封装

## socket函数库

## http协议开发

## 分布式协议     ---把要做的业务放在插件中，把系统和业务分开

## 推荐系统	  ---去网上抓取一些用户浏览数据，让我们访问的时候，推荐
