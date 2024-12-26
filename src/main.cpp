#include <iostream>
#include <asio.hpp>
#include "server.h"

int main() {
    try {
        asio::io_context io_context;

        // 创建并启动HTTP服务器，监听端口8080
        HttpServer server(io_context, 8080);
        std::cout << "HTTP Server started on port 8080." << std::endl;

        server.start();
        io_context.run();  // 启动事件循环，处理异步I/O

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
