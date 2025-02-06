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
    add_definitions(-DPLATFORM_WINDOWS)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    message(">> Platform: Linux")
    add_definitions(-DPLATFORM_LINUX)
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

set(LIB_SRC_LOG 
    src/_log/log.cpp 
    src/util/util.cpp
)

# 创建共享库
add_library(log SHARED ${LIB_SRC_LOG})
set_target_properties(log PROPERTIES 
    OUTPUT_NAME "log"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

add_library(log_static STATIC ${LIB_SRC_LOG})
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



set(LIB_SRC_CONFIG 
    src/config/config.cpp
)

# 创建共享库
add_library(config SHARED ${LIB_SRC_CONFIG})
set_target_properties(config PROPERTIES 
    OUTPUT_NAME "config"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

add_library(config_static STATIC ${LIB_SRC_CONFIG})
set_target_properties(config_static PROPERTIES 
    OUTPUT_NAME "config_static"
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)

# 判断平台
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
message(">> Platform: Windows")

target_include_directories(config
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/_log
    ${CMAKE_SOURCE_DIR}/src/util
    "D:\\libs\\yaml-cpp-0.8.0\\include"
    PUBLIC
    "D:\\libs\\boost_1_87_0"
)

target_include_directories(config_static
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/_log
    ${CMAKE_SOURCE_DIR}/src/util
    "D:\\libs\\yaml-cpp-0.8.0\\include"
    PUBLIC
    "D:\\libs\\boost_1_87_0"
)

target_link_directories(config PRIVATE 
    "D:\\libs\\yaml-cpp-0.8.0\\lib"
)
target_link_directories(config_static PRIVATE 
    "D:\\libs\\yaml-cpp-0.8.0\\lib"
)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
message(">> Platform: Linux")

target_include_directories(config
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/_log
    ${CMAKE_SOURCE_DIR}/src/util
    # "D:\\libs\\yaml-cpp-0.8.0\\include"
    PUBLIC
)

target_include_directories(config_static
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/_log
    ${CMAKE_SOURCE_DIR}/src/util
    # "D:\\libs\\yaml-cpp-0.8.0\\include"
    PUBLIC
)

target_link_directories(config PRIVATE 
    # "D:\\libs\\yaml-cpp-0.8.0\\lib"
)
target_link_directories(config_static PRIVATE 
    # "D:\\libs\\yaml-cpp-0.8.0\\lib"
)

else()
    message(">> unknow.")
endif()

target_include_directories(config
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/_log
    ${CMAKE_SOURCE_DIR}/src/util
    "D:\\libs\\yaml-cpp-0.8.0\\include"
    PUBLIC
    "D:\\libs\\boost_1_87_0"
)

target_include_directories(config_static
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/_log
    ${CMAKE_SOURCE_DIR}/src/util
    "D:\\libs\\yaml-cpp-0.8.0\\include"
    PUBLIC
    "D:\\libs\\boost_1_87_0"
)

target_link_directories(config PRIVATE 
    "D:\\libs\\yaml-cpp-0.8.0\\lib"
)
target_link_directories(config_static PRIVATE 
    "D:\\libs\\yaml-cpp-0.8.0\\lib"
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
target_compile_definitions(config PRIVATE YAML_CPP_STATIC_DEFINE)
target_compile_definitions(config_static PRIVATE YAML_CPP_STATIC_DEFINE)

# 创建可执行文件
add_executable(test tests/test.cpp)
target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/src/_log)
set_target_properties(test PROPERTIES 
    OUTPUT_NAME "test"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)
target_link_libraries(test PRIVATE log_static)



add_executable(test_config tests/test_config.cpp)
target_compile_definitions(test_config PRIVATE YAML_CPP_STATIC_DEFINE)
target_include_directories(test_config PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/config
    ${CMAKE_SOURCE_DIR}/src/_log
    "D:\\libs\\yaml-cpp-0.8.0\\include"
)

target_link_directories(test_config PRIVATE 
    "D:\\libs\\yaml-cpp-0.8.0\\lib"
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


