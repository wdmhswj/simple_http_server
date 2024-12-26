#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <asio.hpp>

class HttpServer {
public:
    HttpServer(asio::io_context& io_context, short port);

    void start();
    void stop();

private:
    void accept_connection();
    void handle_request(asio::ip::tcp::socket& socket);

    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
};

#endif // HTTP_SERVER_H
