/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     service_coordinator_server.h                                    *
*  @brief    gRPC impl: service coordinate                                   *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :  proto file: resources/protos/service_coordinator.proto         *
*****************************************************************************/

#ifndef _SERVICE_COORDINATE_SERVER_H_
#define _SERVICE_COORDINATE_SERVER_H_

#include <string>
#include "protos/service_coordinator/service_coordinator.grpc.pb.h"
#include "protos/service_coordinator/service_coordinator.pb.h"
#include "grpc/servers/grpc_server.h"
#include "grpc/servers/grpc_server_builder.h"

class ServiceCoordinatorServer: public serviceCoordinator::Communicate::Service {
public:
    ServiceCoordinatorServer();
    virtual ~ServiceCoordinatorServer();

    virtual grpc::Status informPreviousServiceInfo(grpc::ServerContext*, const serviceCoordinator::InformPreviousServiceInfoRequest*, serviceCoordinator::InformPreviousServiceInfoResponse*) override;
private:
};

#endif /* _SERVICE_COORDINATE_SERVER_H_ */
