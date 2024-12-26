#include "server.h"
#include <iostream>
#include <asio.hpp>
#include <thread>

HttpServer::HttpServer(asio::io_context& io_context, short port)
    : io_context_(io_context), acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

void HttpServer::start() {
    accept_connection();
}

void HttpServer::stop() {
    acceptor_.close();
}

void HttpServer::accept_connection() {
    acceptor_.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
        if (!ec) {
            std::cout << "New connection accepted!" << std::endl;
            std::thread(&HttpServer::handle_request, this, std::move(socket)).detach();
        }
        accept_connection(); // 继续接受其他连接
    });
}

void HttpServer::handle_request(asio::ip::tcp::socket& socket) {
    try {
        char data[1024];
        asio::error_code error;

        // 读取请求数据
        size_t length = socket.read_some(asio::buffer(data), error);
        if (error && error != asio::error::eof) {
            throw std::system_error(error);
        }

        std::string request(data, length);
        std::cout << "Request received: \n" << request << std::endl;

        // 响应客户端
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
        asio::write(socket, asio::buffer(response), error);

        socket.close();
    } catch (const std::exception& e) {
        std::cerr << "Error handling request: " << e.what() << std::endl;
    }
}
