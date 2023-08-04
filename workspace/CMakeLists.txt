cmake_minimum_required(VERSION 2.8)
project(yy)

set(CMAKE_VERBOSE_MAKEFILE ON)
#下面是在make过程中显示的详细命令
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function")

#把源码编译到LIB_SRC 上面
set(LIB_SRC
    src/logger.cpp
    )

#加一个 SHARED库，生成.so 文件
add_library(yy SHARED ${LIB_SRC})
#add_library(yy_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (yy_static PROPERTIES OUTPUT_NAME "yy")

add_executable(test tests/test.cpp)
add_dependencies(test yy)
target_link_libraries(test yy)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
