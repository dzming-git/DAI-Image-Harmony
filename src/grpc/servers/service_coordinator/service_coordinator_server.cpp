#include "grpc/servers/service_coordinator/service_coordinator_server.h"

ServiceCoordinatorServer::ServiceCoordinatorServer() {
}

ServiceCoordinatorServer::~ServiceCoordinatorServer() {
}
grpc::Status ServiceCoordinatorServer::informPreviousServiceInfo(grpc::ServerContext*, const serviceCoordinator::InformPreviousServiceInfoRequest *request, serviceCoordinator::InformPreviousServiceInfoResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    std::cout << "grpc::Status ServiceCoordinatorServer::informPreviousServiceInfo" << std::endl;
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}
