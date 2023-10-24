#include "grpc/servers/task_coordinate/task_coordinate_server.h"

TaskCoordinateServer::TaskCoordinateServer() {
}

TaskCoordinateServer::~TaskCoordinateServer() {
}
grpc::Status TaskCoordinateServer::informPreviousServiceInfo(grpc::ServerContext*, const taskCoordinate::InformPreviousServiceInfoRequest *request, taskCoordinate::InformPreviousServiceInfoResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    std::cout << "grpc::Status TaskCoordinateServer::informPreviousServiceInfo" << std::endl;
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}
