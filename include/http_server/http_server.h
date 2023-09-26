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
#include <arpa/inet.h>

class HttpServer {
public:
    class Builder;
    std::string getHost();
    uint16_t getPort();
    void start();
    ~HttpServer();
private:
    HttpServer(HttpServer::Builder* builder);

    std::string host;
    uint16_t port;
    int fd; // 套接字
    struct sockaddr_in address;  // 地址
    int newSocket;
};

#endif /* _HTTP_SERVER_H_ */
