/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     grpc_server.h                                                   *
*  @brief    gRPC Server                                                     *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :                                                                 *
*****************************************************************************/
#ifndef _GRPC_SERVER_H_
#define _GRPC_SERVER_H_

#include <grpcpp/grpcpp.h>
#include <vector>

class GRPCServer {
public:
    class GRPCServerBuilder;
    void start();
private:
    GRPCServer(GRPCServerBuilder*, grpc::ServerBuilder*);
    std::unique_ptr<grpc::Server> server;
};

#endif /* _GRPC_SERVER_H_ */
