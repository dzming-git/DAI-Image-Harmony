/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     service_coordinate_server.h                                     *
*  @brief    gRPC impl: service coordinate                                   *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :  proto file: resources/protos/service_coordinate.proto          *
*****************************************************************************/

#ifndef _SERVICE_COORDINATE_SERVER_H_
#define _SERVICE_COORDINATE_SERVER_H_

#include <string>
#include "protos/service_coordinate/service_coordinate.grpc.pb.h"
#include "protos/service_coordinate/service_coordinate.pb.h"
#include "grpc/servers/grpc_server.h"
#include "grpc/servers/grpc_server_builder.h"

class ServiceCoordinateServer: public serviceCoordinate::Communicate::Service {
public:
    ServiceCoordinateServer();
    virtual ~ServiceCoordinateServer();

    virtual grpc::Status informPreviousServiceInfo(grpc::ServerContext*, const serviceCoordinate::InformPreviousServiceInfoRequest*, serviceCoordinate::InformPreviousServiceInfoResponse*) override;
private:
};

#endif /* _SERVICE_COORDINATE_SERVER_H_ */
