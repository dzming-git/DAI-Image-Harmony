/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     grpc_server_builder                                             *
*  @brief    build gRPC Server                                               *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :                                                                 *
*****************************************************************************/

#ifndef _GRPC_SERVER_BUILDER_H_
#define _GRPC_SERVER_BUILDER_H_

#include <grpcpp/grpcpp.h>
#include <vector>
#include "grpc/grpc_server.h"

class GRPCServer::GRPCServerBuilder {
public:
    GRPCServerBuilder();
    ~GRPCServerBuilder();
    GRPCServerBuilder& setHost(std::string);
    std::string getHost();
    GRPCServerBuilder& setPort(std::string);
    std::string getPort();
    GRPCServerBuilder& addService(grpc::Service*);
    GRPCServerBuilder& setMaxSendBytes(int);
    GRPCServerBuilder& setEpollCount(int, int);
    GRPCServer* build();
private:
    grpc::ServerBuilder* serverBuilder;
    std::string host;
    std::string port;
};

#endif /* _GRPC_SERVER_BUILDER_H_ */
