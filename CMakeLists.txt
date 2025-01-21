cmake_minimum_required(VERSION 3.10)
project(SimpleHttpServer)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_BUILD_TYPE Debug)

# 从环境变量中获取 CXXFLAGS（如果定义）
if(DEFINED ENV{CXXFLAGS})
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")
else()
    set(CMAKE_CXX_FLAGS "-O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")
endif()

# 查看编译器类型
message("CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

# 判断平台
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    message(">> Platform: Windows")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    message(">> Platform: Linux")
else()
    message(">> unknow.")
endif()

# 判断编译器
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(">> using Clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(">> using GCC")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    message(">> using Intel C++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(">> using Visual Studio C++")
else()
    message(">> unknow compiler.")
endif()

set(LIB_SRC 
    src/_log/log.cpp 
    src/util/util.cpp
    )

# 创建共享库
add_library(shs SHARED ${LIB_SRC})
set_target_properties(shs PROPERTIES 
    OUTPUT_NAME "shs"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

add_library(shs_static STATIC ${LIB_SRC})
set_target_properties(shs_static PROPERTIES 
    OUTPUT_NAME "shs"
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)


target_include_directories(shs PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/_log
    ${CMAKE_SOURCE_DIR}/src/util
    )
# add_library(shs STATIC ${LIB_SRC})

# 创建可执行文件
add_executable(test tests/test.cpp)

set_target_properties(test PROPERTIES 
    OUTPUT_NAME "test"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# 设置依赖关系和链接库
# add_dependencies(test shs)
target_link_libraries(test PRIVATE shs)

# # 设置输出路径
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


