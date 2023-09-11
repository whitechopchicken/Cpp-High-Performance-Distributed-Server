cmake_minimum_required(VERSION 2.8)
project(yy)

include(cmake/utils.cmake)

set(CMAKE_VERSION_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -ggdb -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)
#yaml-cpp

find_library(YAMLCPP yaml-cpp)
message("***", ${YAMLCPP})

set(LIB_SRC 
    src/log.cpp 
    src/util.cpp
    src/thread.cpp
    src/config.cpp
    src/fiber.cpp
    src/scheduler.cpp
    ) 
#头文件会自己去找？

add_library(yy SHARED ${LIB_SRC})  #创建动态链接库 yy ，资源文件是LIB_SRC
force_redefine_file_macro_for_sources(yy)   # 重定向 __FILE__

set(LIB_LIB
    yy
    pthread
    ${YAMLCPP}
)

add_executable(test1 tests/test.cpp  "${LIBS}")   # LIBS这个变量未定义啊？ 去除yy
add_dependencies(test1 yy)
force_redefine_file_macro_for_sources(test)
target_link_libraries(test1  ${LIB_LIB})  #这样写会不会重复包含 yy 这个库文件？==>去除ok

add_executable(test_config tests/test_config.cpp  "${LIBS}")
add_dependencies(test_config yy)
force_redefine_file_macro_for_sources(test_config)
target_link_libraries(test_config  ${LIB_LIB})

add_executable(test_util tests/test_util.cpp  "${LIBS}")
add_dependencies(test_util yy)
force_redefine_file_macro_for_sources(test_util)
target_link_libraries(test_util  ${LIB_LIB})

add_executable(test_thread tests/test_thread.cpp  "${LIBS}")
add_dependencies(test_thread yy )
force_redefine_file_macro_for_sources(test_thread)
target_link_libraries(test_thread  ${LIB_LIB})

add_executable(test_fiber tests/test_fiber.cpp "${LIBS}")
add_dependencies(test_fiber yy)
force_redefine_file_macro_for_sources(test_fiber)
target_link_libraries(test_fiber ${LIB_LIB})

add_executable(test_scheduler tests/test_scheduler.cpp "${LIBS}")
add_dependencies(test_scheduler yy)
force_redefine_file_macro_for_sources(test_scheduler)
target_link_libraries(test_scheduler ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
