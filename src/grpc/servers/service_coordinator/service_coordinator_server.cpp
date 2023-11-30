#include "grpc/servers/service_coordinator/service_coordinator_server.h"
#include "image_loaders/image_loader_factory.h"
#include "image_loaders/image_loader_controller.h"
#include <chrono>
#include <random>
#include <unordered_map>

ServiceCoordinatorServer::ServiceCoordinatorServer() {
}

ServiceCoordinatorServer::~ServiceCoordinatorServer() {
}
grpc::Status ServiceCoordinatorServer::informPreviousServiceInfo(grpc::ServerContext*, const serviceCoordinator::InformPreviousServiceInfoRequest *request, serviceCoordinator::InformPreviousServiceInfoResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    try {
        // 当前版本该服务没有前置服务
        throw std::runtime_error("The current version of this service does not have a front-end service.\n");
    } catch (const std::exception& e) {
        responseCode = 400;
        responseMessage += e.what();
    }
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}

grpc::Status ServiceCoordinatorServer::informCurrentServiceInfo(grpc::ServerContext*, const serviceCoordinator::InformCurrentServiceInfoRequest* request, serviceCoordinator::InformCurrentServiceInfoResponse* response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    try {
        std::unordered_map<std::string, std::string> args;
        int argsCnt = request->args_size();
        for (int i = 0; i < argsCnt; ++i) {
            auto arg = request->args(i);
            args.emplace(arg.key(), arg.value());
        }

        if (args.find("SourceType") == args.end()) {
            throw std::runtime_error("SourceType is not set.\n");
        }

        ImageLoaderFactory::SourceType sourceType; 
        auto imgTypeIt = ImageLoaderFactory::sourceTypeMap.find(args["SourceType"]);
        if (ImageLoaderFactory::sourceTypeMap.end() == imgTypeIt) {
            // 匹配不到，用编辑距离智能匹配
            std::string mostSimilarType = ImageLoaderFactory::getMostSimilarSourceType(args["SourceType"]);
            sourceType = ImageLoaderFactory::sourceTypeMap[mostSimilarType];
            responseMessage += "Cannot match " + args["SourceType"] + ". The closest match is " + mostSimilarType + ".\n";
        }
        else {
            sourceType = imgTypeIt->second;
            responseMessage += "Match.\n";
        }
        auto imageLoaderController = ImageLoaderController::getSingletonInstance();
        int64_t connectId = imageLoaderController->registerImageLoader(args, sourceType);
        if (-1 == connectId) {
            throw std::runtime_error("Register image loader failed.\n");
        }
        serviceCoordinator::Argument argument;
        argument.set_key("ConnectID");
        argument.set_value(std::to_string(connectId));
        response->add_args()->CopyFrom(argument);
    } catch (const std::exception& e) {
        responseCode = 400;
        responseMessage += e.what();
    }

    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}
