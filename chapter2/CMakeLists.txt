# #cmake 最小版本要求
# cmake_minimum_required(VERSION 2.8)

# #项目名称为yy
# project(yy)

# #开启详细的生成信息
# set(CMAKE_VERBOSE_MAKEFILE ON)

# #设置C++编译器的编译选项，使用环境变量 $ENV{CXXFLAGS} 来获取可能在环境中定义的额外编译选项
# set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -ggdb -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

# #添加头文件搜索路径，包括当前目录和/usr/local/include
# include_directories(.)
# include_directories(/usr/local/include)

# #添加库文件搜索路径。
# link_directories(/usr/local/lib)

# #查找并设置'YAMLCPP' 变量为 'yaml-cpp' 库的路径，找到的库将用于后面的链接   
# find_library(YAMLCPP yaml-cpp)

# #向输出中打印'YAMLCPP'的值，应该为找到的库的路径
# message("***", ${YAMLCPP})

# #定义一个'LIB_SRC' 变量，其中包含一些源代码文件的路径
# set(LIB_SRC ./src/log.cpp ./src/config.cpp ./src/util.cpp)

# #构建一个名为 yy 的共享库，其源文件由 LIB_SRC 变量指定
# add_library(yy SHARED ${LIB_SRC})

# #构建一个名为 test1 的可执行文件，其源文件为'tests/test.cpp '  --删除后面部分-- 和一些其他文件，比如 yy库 和 LIBS   <==> LIBS没定义 
# #需要在 后面加一个 "${LIBS}" ,但不需要yy ！！！
# add_executable(test1 tests/test.cpp )

# #将 'test1' 与 'yy' 库建立依赖关系
# add_dependencies(test1 yy)

# #为'test1' 链接需要的库， 包括 yy 和 YAMLCPP
# target_link_libraries(test1 yy ${YAMLCPP})

# #类似地， 构建 'test_config' 可执行文件， 并链接yy 和 YAMLCPP
# add_executable(test_config tests/test_config.cpp)
# add_dependencies(test_config yy)
# target_link_libraries(test_config yy ${YMALCPP})

# #设置输出路径
# SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

cmake_minimum_required(VERSION 2.8)
project(yy)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_library(YAMLCPP yaml-cpp)
message("***", ${YAMLCPP})

set(LIB_SRC
    src/log.cpp
    src/util.cpp
    src/config.cpp
    )

add_library(yy SHARED ${LIB_SRC})


# add_executable(test1 "tests/test.cpp" yy "${LIBS}")
# add_dependencies(test1 yy)
# target_link_libraries(test1 yy)

add_executable(test1 tests/test.cpp  "${LIBS}")
add_dependencies(test1 yy)
target_link_libraries(test1 yy ${YAMLCPP})

add_executable(test_config tests/test_config.cpp  "${LIBS}")
add_dependencies(test_config yy)
target_link_libraries(test_config yy ${YAMLCPP})


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)