cmake_minimum_required(VERSION 3.0)
project(SimpleHttpServer)


set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV(CXXFLAGS) -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

if(NOT MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
endif()

set(LIB_SRC src/log.cpp)

add_library(shs SHARED ${LIB_SRC})

add_executable(test tests/test.cpp)
add_dependencies(test shs)
target_link_libraries(test shs)

set(EXECUTABLE_OUTPUT_PATH $(PROJECT_SOURCE_DIR)/bin)
set(LIBRARY_OUTPUT_PATH $(PROJECT_SOURCE_DIR)/lib)