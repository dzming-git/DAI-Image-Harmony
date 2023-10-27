#include "grpc/servers/service_coordinate/service_coordinate_server.h"

ServiceCoordinateServer::ServiceCoordinateServer() {
}

ServiceCoordinateServer::~ServiceCoordinateServer() {
}
grpc::Status ServiceCoordinateServer::informPreviousServiceInfo(grpc::ServerContext*, const serviceCoordinate::InformPreviousServiceInfoRequest *request, serviceCoordinate::InformPreviousServiceInfoResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    std::cout << "grpc::Status ServiceCoordinateServer::informPreviousServiceInfo" << std::endl;
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}
