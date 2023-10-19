#include "grpc/services/task_coordinate/task_coordinate.h"

TaskCoordinateService::TaskCoordinateService() {
}

TaskCoordinateService::~TaskCoordinateService() {
}
grpc::Status TaskCoordinateService::informPreviousServiceInfo(grpc::ServerContext*, const taskCoordinate::InformPreviousServiceInfoRequest *request, taskCoordinate::InformPreviousServiceInfoResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    std::cout << "grpc::Status TaskCoordinateService::informPreviousServiceInfo" << std::endl;
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}
