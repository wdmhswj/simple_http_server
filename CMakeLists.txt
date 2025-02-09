cmake_minimum_required(VERSION 3.10)
project(SimpleHttpServer)

include(cmake/utils.cmake)

# yaml-cpp 使用静态库
add_definitions(-DYAML_CPP_STATIC_DEFINE)
# 在 CMakeLists.txt 中禁用 'builtin-macro-redefined' 警告
add_definitions(-Wno-builtin-macro-redefined)


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
    add_definitions(-DPLATFORM_WINDOWS)
   
    # 添加 Windows系统下 安装的第三方库所在目录
    include_directories("D:\\libs\\include")
    link_directories("D:\\libs\\lib")

elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    message(">> Platform: Linux")
    add_definitions(-DPLATFORM_LINUX)
else()
    message(">> unknow.")
endif()

include_directories("${PROJECT_SOURCE_DIR}")

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


# log

set(LIB_SRC_LOG 
    src/_log/log.cpp 
    src/util/util.cpp
)

# 创建共享库
add_library(log SHARED ${LIB_SRC_LOG})

# 将 __FILE__ 宏改为相对路径
redefine_file_macro(log)

set_target_properties(log PROPERTIES 
    OUTPUT_NAME "log"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

add_library(log_static STATIC ${LIB_SRC_LOG})

# 将 __FILE__ 宏改为相对路径
redefine_file_macro(log_static)

set_target_properties(log_static PROPERTIES 
    OUTPUT_NAME "log_static"
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)



target_include_directories(log PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/_log
    ${CMAKE_SOURCE_DIR}/src/util
)

target_include_directories(log_static PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/_log
    ${CMAKE_SOURCE_DIR}/src/util
)

target_link_directories(log PRIVATE 
)
target_link_directories(log_static PRIVATE 
)

target_link_libraries(log PRIVATE
    yaml-cpp
)

target_link_libraries(log_static PRIVATE
    yaml-cpp
)

# config

set(LIB_SRC_CONFIG 
    src/config/config.cpp
)

# 创建共享库
add_library(config SHARED ${LIB_SRC_CONFIG})

# 将 __FILE__ 宏改为相对路径
redefine_file_macro(config)

set_target_properties(config PROPERTIES 
    OUTPUT_NAME "config"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

add_library(config_static STATIC ${LIB_SRC_CONFIG})

# 将 __FILE__ 宏改为相对路径
redefine_file_macro(config_static)

set_target_properties(config_static PROPERTIES 
    OUTPUT_NAME "config_static"
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)

target_link_libraries(config
    PRIVATE
    yaml-cpp
    log
)
target_link_libraries(config_static
    PRIVATE
    yaml-cpp
    log
)

# thread

set(LIB_SRC_THREAD 
    src/thread/thread.cpp
)

# 创建共享库
add_library(thread SHARED ${LIB_SRC_THREAD})

# 将 __FILE__ 宏改为相对路径
redefine_file_macro(thread)

set_target_properties(thread PROPERTIES 
    OUTPUT_NAME "thread"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

add_library(thread_static STATIC ${LIB_SRC_THREAD})

# 将 __FILE__ 宏改为相对路径
redefine_file_macro(thread_static)

set_target_properties(thread_static PROPERTIES 
    OUTPUT_NAME "thread_static"
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)

target_link_libraries(thread
    PRIVATE
    log
)
target_link_libraries(thread_static
    PRIVATE
    log
)

# test

# 创建可执行文件
add_executable(test tests/test.cpp)

# 将 __FILE__ 宏改为相对路径
redefine_file_macro(test)

target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/src/_log)
# target_compile_definitions(test PRIVATE YAML_CPP_STATIC_DEFINE)
set_target_properties(test PROPERTIES 
    OUTPUT_NAME "test"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)
target_link_libraries(test PRIVATE log_static)

# test_config

add_executable(test_config tests/test_config.cpp)

# 将 __FILE__ 宏改为相对路径
redefine_file_macro(test_config)

# target_compile_definitions(test_config PRIVATE YAML_CPP_STATIC_DEFINE)
target_include_directories(test_config PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/config
    ${CMAKE_SOURCE_DIR}/src/_log
)

target_link_directories(test_config PRIVATE 
)

set_target_properties(test_config PROPERTIES 
    OUTPUT_NAME "test_config"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

target_link_libraries(test_config PRIVATE
    config_static
    log_static
    yaml-cpp
)

# test_thread

add_executable(test_thread tests/test_thread.cpp)

# 将 __FILE__ 宏改为相对路径
redefine_file_macro(test_thread)

# target_compile_definitions(test_thread PRIVATE YAML_CPP_STATIC_DEFINE)
target_include_directories(test_thread PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/_log
)

target_link_directories(test_thread PRIVATE 
)

set_target_properties(test_thread PROPERTIES 
    OUTPUT_NAME "test_thread"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

target_link_libraries(test_thread PRIVATE
    thread_static
)