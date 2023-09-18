/*****************************************************************************
*  Copyright © 2023 - 2023 dzming  dzm_work@163.com.                         *
*                                                                            *
*  @file     http_server.h                                                   *
*  @brief    HTTP服务器                                                       *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*****************************************************************************/

#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

class HttpServer {
public:
    class Builder;
    std::string getHost();
    uint16_t getPort();
    void start();
private:
    HttpServer(HttpServer::Builder* builder);

    std::string host;
    uint16_t port;
    int fd; // 套接字
    struct sockaddr_in address;  // 地址
};

class HttpServer::Builder {
public:
    Builder();
    std::string getHost();
    uint16_t getPort();
    void setHost(std::string host);
    void setPort(uint16_t port);
    HttpServer* build();
private:
    std::string host;
    uint16_t port;
};

#endif /* _HTTP_SERVER_H_ */