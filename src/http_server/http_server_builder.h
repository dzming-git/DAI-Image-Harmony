/*****************************************************************************
*  Copyright © 2023 - 2023 dzming  dzm_work@163.com.                         *
*                                                                            *
*  @file     http_server_builder.h                                           *
*  @brief    Build Http Server                                               *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*****************************************************************************/

#ifndef _HTTP_SERVER_BUILDER_H_
#define _HTTP_SERVER_BUILDER_H_

#include "http_server/http_server.h"

class HttpServer::Builder {
public:
    Builder();
    std::string getHost();
    uint16_t getPort();
    HttpServer::Builder& setHost(std::string host);
    HttpServer::Builder& setPort(uint16_t port);
    HttpServer* build();
private:
    std::string host;
    uint16_t port;
};

#endif /* _HTTP_SERVER_BUILDER_H_ */
