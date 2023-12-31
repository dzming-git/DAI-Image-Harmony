#include "grpc/servers/image_harmony/image_harmony_server.h"
#include "image_loaders/image_loader_factory.h"
#include "image_loaders/image_loader_controller.h"
#include <chrono>
#include <random>
#include <vector>

ImageHarmonyServer::ImageHarmonyServer() {
}

ImageHarmonyServer::~ImageHarmonyServer() {
}

grpc::Status ImageHarmonyServer::registerImageTransService(grpc::ServerContext*, const imageHarmony::RegisterImageTransServiceRequest *request, imageHarmony::RegisterImageTransServiceResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    try {
        response->set_connectid(0);
        int64_t loaderArgsHash = request->loaderargshash();
        bool isUnique = request->isunique();
        std::unordered_map<std::string, std::string> args;
        ImageLoaderFactory::SourceType sourceType;
        if (0 == loaderArgsHash) {
            auto sourceTypeIt = ImageLoaderFactory::sourceTypeMap.find(request->sourcetype());
            if (ImageLoaderFactory::sourceTypeMap.end() == sourceTypeIt) {
                // 匹配不到，用编辑距离智能匹配
                std::string mostSimilarType = ImageLoaderFactory::getMostSimilarSourceType(request->sourcetype());
                sourceType = ImageLoaderFactory::sourceTypeMap[mostSimilarType];
                responseMessage += "Cannot match " + request->sourcetype() + ". The closest match is " + mostSimilarType + ".\n";
            }
            else {
                sourceType = sourceTypeIt->second;
                responseMessage += "Match.\n";
            }
            int argsCnt = request->args_size();
            for (int i = 0; i < argsCnt; ++i) {
                auto arg = request->args(i);
                args.emplace(arg.key(), arg.value());
            }
        }
        auto imageLoaderController = ImageLoaderController::getSingletonInstance();
        int64_t connectId = 0;
        bool ok = imageLoaderController->registerImageLoader(args, sourceType, loaderArgsHash, connectId, isUnique);
        response->set_loaderargshash(loaderArgsHash);
        response->set_connectid(connectId);
        if (!ok) {
            throw std::runtime_error("Register image loader failed.\n");
        }
    } catch (const std::exception& e) {
        responseCode = 400;
        responseMessage += e.what();
    }

    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}

grpc::Status ImageHarmonyServer::unregisterImageTransService(grpc::ServerContext *context, const imageHarmony::UnregisterImageTransServiceRequest *request, imageHarmony::UnregisterImageTransServiceResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    try {
        int64_t connectId = request->connectid();
        auto imageLoaderController = ImageLoaderController::getSingletonInstance();
        if (false == imageLoaderController->unregisterImageLoader(connectId)) {
            throw std::runtime_error("unregister image loader failed.\n");
        }
    } catch (const std::exception& e) {
        responseCode = 400;
        responseMessage += e.what();
    }
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}

grpc::Status ImageHarmonyServer::getImageByImageId(grpc::ServerContext *context, const imageHarmony::GetImageByImageIdRequest *request, imageHarmony::GetImageByImageIdResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;      
    try {
        int64_t connectId = request->connectid();
        auto imageRequest = request->imagerequest();
        int64_t imageId = imageRequest.imageid();
        bool noImageBuffer = imageRequest.noimagebuffer();
        std::string format = imageRequest.format();
        int paramsCnt = imageRequest.params_size();
        std::vector<int> params(paramsCnt);
        for (int i = 0; i < paramsCnt; ++i) {
            params[i] = imageRequest.params(i);
        }
        int expectedW = imageRequest.expectedw();
        int expectedH = imageRequest.expectedh();
        response->mutable_imageresponse()->set_imageid(0);
        auto imageLoaderController = ImageLoaderController::getSingletonInstance();
        auto imageLoader = imageLoaderController->getImageLoader(connectId);
        if (nullptr == imageLoader) {
            throw std::runtime_error("The imageLoader is nullptr. Unable to load the image.\n");
        }
        imageLoaderController->startUsingLoader(connectId);
        auto imageInfo = imageLoader->getImageById(imageId);
        if (0 == imageInfo.imageId || imageInfo.image.empty()) {
            imageLoaderController->stopUsingLoader(connectId);
            throw std::runtime_error("Image is NULL.\n");
        }
        imageLoaderController->stopUsingLoader(connectId);
        response->mutable_imageresponse()->set_imageid(imageInfo.imageId);
        response->mutable_imageresponse()->set_height(imageInfo.image.rows);
        response->mutable_imageresponse()->set_width(imageInfo.image.cols);
        if (!noImageBuffer) {
            if (expectedW * expectedH && !(expectedW == imageInfo.image.cols && expectedH == imageInfo.image.rows)) {
                double aspectRatio = (double)imageInfo.image.cols / imageInfo.image.rows;
                double expectedAspectRatio = (double)expectedW / expectedH;
                if (abs(aspectRatio - expectedAspectRatio) > 0.01) {
                    std::string originalAspectRatio = std::to_string(aspectRatio);
                    std::string currentAspectRatio = std::to_string(expectedAspectRatio);
                    responseMessage += "The aspect ratio has changed. Original aspect ratio: " + originalAspectRatio + ". Current aspect ratio: " + currentAspectRatio + ".\n";
                }
                cv::resize(imageInfo.image, imageInfo.image, cv::Size(expectedW, expectedH));
            }
            response->mutable_imageresponse()->set_height(imageInfo.image.rows);
            response->mutable_imageresponse()->set_width(imageInfo.image.cols);
            // 无参数时，默认使用 .jpg 无损压缩
            if (0 == format.size()) {
                format = ".jpg";
                params = {cv::IMWRITE_PNG_COMPRESSION, 100};
            }
            std::vector<uchar> buf;
            // TODO: 在这里压缩图像会有一些性能冗余
            cv::imencode(format, imageInfo.image, buf, params);
            size_t bufSize = buf.size();
            response->mutable_imageresponse()->set_buffer(&buf[0], buf.size());
        }
    } catch (const std::exception& e) {
        responseCode = 400;
        responseMessage += e.what();
    }
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}

grpc::Status ImageHarmonyServer::getNextImageByImageId(grpc::ServerContext *context, const imageHarmony::GetNextImageByImageIdRequest *request, imageHarmony::GetNextImageByImageIdResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    try {
        int64_t connectId = request->connectid();
        auto imageRequest = request->imagerequest();
        int64_t imageId = imageRequest.imageid();
        std::string format = imageRequest.format();
        bool noImageBuffer = imageRequest.noimagebuffer();
        int paramsCnt = imageRequest.params_size();
        std::vector<int> params(paramsCnt);
        for (int i = 0; i < paramsCnt; ++i) {
            params[i] = imageRequest.params(i);
        }
        int expectedW = imageRequest.expectedw();
        int expectedH = imageRequest.expectedh();
        response->mutable_imageresponse()->set_imageid(0);        

        auto imageLoaderController = ImageLoaderController::getSingletonInstance();
        auto imageLoader = imageLoaderController->getImageLoader(connectId);
        if (nullptr == imageLoader) {
            throw std::runtime_error("The imageLoader is nullptr. Unable to load the image.\n");
        }
        
        imageLoaderController->startUsingLoader(connectId);
        if (0 == imageId && !imageLoader->hasNext()) {
            imageLoaderController->stopUsingLoader(connectId);
            throw std::runtime_error("Image is NULL.\n");
        }
        auto imageInfo = imageLoader->next(imageId);
        imageLoaderController->stopUsingLoader(connectId);
        if (0 == imageInfo.imageId || imageInfo.image.empty()) {
            throw std::runtime_error("Image is NULL.\n");
        }
        response->mutable_imageresponse()->set_imageid(imageInfo.imageId);
        response->mutable_imageresponse()->set_height(imageInfo.image.rows);
        response->mutable_imageresponse()->set_width(imageInfo.image.cols);
        if (!noImageBuffer) {
            if (expectedW * expectedH && !(expectedW == imageInfo.image.cols && expectedH == imageInfo.image.rows)) {
                double aspectRatio = (double)imageInfo.image.cols / imageInfo.image.rows;
                double expectedAspectRatio = (double)expectedW / expectedH;
                if (abs(aspectRatio - expectedAspectRatio) > 0.01) {
                    std::string originalAspectRatio = std::to_string(aspectRatio);
                    std::string currentAspectRatio = std::to_string(expectedAspectRatio);
                    responseMessage += "The aspect ratio has changed. Original aspect ratio: " + originalAspectRatio + ". Current aspect ratio: " + currentAspectRatio + ".\n";
                }
                cv::resize(imageInfo.image, imageInfo.image, cv::Size(expectedW, expectedH));
            }
            response->mutable_imageresponse()->set_height(imageInfo.image.rows);
            response->mutable_imageresponse()->set_width(imageInfo.image.cols);
            // 无参数时，默认使用 .jpg 无损压缩
            if (0 == format.size()) {
                format = ".jpg";
                params = {cv::IMWRITE_PNG_COMPRESSION, 100};
            }
            std::vector<uchar> buf;
            // TODO: 在这里压缩图像会有一些性能冗余
            cv::imencode(format, imageInfo.image, buf, params);
            size_t bufSize = buf.size();
            response->mutable_imageresponse()->set_buffer(&buf[0], buf.size());
        }
    } catch (const std::exception& e) {
        responseCode = 400;
        responseMessage += e.what();
    }
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}
