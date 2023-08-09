cmake_minimum_required(VERSION 2.8)
project(yy)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -ggdb -std=c++17 -Wall -Wno-deprecated -Wno-unused-function -Wno-builtin-macro-redefined")

#设置lib

set(LIB_SRC src/log.cpp src/util.cpp )

#添加lib

add_library(yy SHARED ${LIB_SRC})

add_executable(test01 "tests/test.cpp" )
add_dependencies(test01 yy)
target_link_libraries(test01 yy)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)