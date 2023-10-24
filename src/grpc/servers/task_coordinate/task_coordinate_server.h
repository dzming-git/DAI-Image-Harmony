/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     task_coordinate_server.h                                        *
*  @brief    gRPC impl: task coordinate                                      *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :  proto file: resources/protos/task_coordinate.proto             *
*****************************************************************************/

#ifndef _TASK_COORDINATE_SERVER_H_
#define _TASK_COORDINATE_SERVER_H_

#include <string>
#include "protos/task_coordinate/task_coordinate.grpc.pb.h"
#include "protos/task_coordinate/task_coordinate.pb.h"
#include "grpc/servers/grpc_server.h"
#include "grpc/servers/grpc_server_builder.h"

class TaskCoordinateServer: public taskCoordinate::Communicate::Service {
public:
    TaskCoordinateServer();
    virtual ~TaskCoordinateServer();

    virtual grpc::Status informPreviousServiceInfo(grpc::ServerContext*, const taskCoordinate::InformPreviousServiceInfoRequest*, taskCoordinate::InformPreviousServiceInfoResponse*) override;
private:
};

#endif /* _TASK_COORDINATE_SERVER_H_ */
