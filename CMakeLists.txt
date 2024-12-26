cmake_minimum_required(VERSION 3.10)
project(SimpleHttpServer)

set(CMAKE_CXX_STANDARD 17)

# 添加asio库路径（如果安装在默认位置，CMake可以自动找到）
find_package(Asio REQUIRED)

# 创建可执行文件
add_executable(simple_http_server src/main.cpp src/server.cpp)

# 链接asio库
target_link_libraries(simple_http_server PRIVATE asio::asio)
