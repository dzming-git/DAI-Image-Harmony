#include "http_server/http_server.h"
#include "http_server/http_server_builder.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <arpa/inet.h>
#include "http_server/http_response_handler/http_response_handler_base.h"
#include "http_server/http_response_handler/http_response_handler_factory.h"

std::string HttpServer::getHost() {
    return host;
}

uint16_t HttpServer::getPort() {
    return port;
}


inline std::string parseUrl(const std::string& request) {
    std::string url;
    size_t start = request.find("GET ") + 4;
    size_t end = request.find(" HTTP/1");

    if (start != std::string::npos && end != std::string::npos) {
        url = request.substr(start, end - start);
    }

    return url;
}

void HttpServer::start() {
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    while (true) {
        if ((newSocket = accept(fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        memset(buffer, 0, sizeof(buffer));
        read(newSocket, buffer, sizeof(buffer));

        std::string request(buffer);
        auto handler = HttpResponseHandlerFactory::createHttpResponseHandler(parseUrl(request));
        std::string response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n404 Not Found";
        if (handler) {
            response = handler->response(request);
        }

        send(newSocket, response.c_str(), response.length(), 0);
        close(newSocket);
    }
}

HttpServer::~HttpServer() {
    if (newSocket != 1) {
        close(newSocket);
    }
}

HttpServer::HttpServer(HttpServer::Builder *builder):host(builder->getHost()), port(builder->getPort()) {
    // 创建套接字
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host.c_str());
    address.sin_port = htons(port);

    // 绑定端口
    if (bind(fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听连接
    if (listen(fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}
