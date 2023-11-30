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

grpc::Status ImageHarmonyServer::registerImgTransService(grpc::ServerContext*, const imageHarmony::RegisterImgTransServiceRequest *request, imageHarmony::RegisterImgTransServiceResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    response->set_connectid(-1);
    ImageLoaderFactory::SourceType imgType;
    auto imgTypeIt = ImageLoaderFactory::sourceTypeMap.find(request->imgtype());
    if (ImageLoaderFactory::sourceTypeMap.end() == imgTypeIt) {
        // 匹配不到，用编辑距离智能匹配
        std::string mostSimilarType = ImageLoaderFactory::getMostSimilarSourceType(request->imgtype());
        imgType = ImageLoaderFactory::sourceTypeMap[mostSimilarType];
        responseMessage = "Cannot match " + request->imgtype() + ". The closest match is " + mostSimilarType + ".\n";
    }
    else {
        imgType = imgTypeIt->second;
        responseMessage = "Match.\n";
    }
    std::unordered_map<std::string, std::string> args;
    int argsCnt = request->args_size();
    for (int i = 0; i < argsCnt; ++i) {
        auto arg = request->args(i);
        args.emplace(arg.key(), arg.value());
    }
    auto imageLoaderController = ImageLoaderController::getSingletonInstance();
    int64_t connectId = imageLoaderController->registerImageLoader(args, imgType);
    response->set_connectid(connectId);
    if (-1 == connectId) {
        responseCode = 400;
        responseMessage += "Register image loader failed.\n";
    }
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}

grpc::Status ImageHarmonyServer::unregisterImgTransService(grpc::ServerContext *context, const imageHarmony::UnregisterImgTransServiceRequest *request, imageHarmony::UnregisterImgTransServiceResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    int64_t connectId = request->connectid();
    auto imageLoaderController = ImageLoaderController::getSingletonInstance();
    if (false == imageLoaderController->unregisterImageLoader(connectId)) {
        responseCode = 400;
        responseMessage = "unregister image loader failed.\n";
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
        int64_t imageId = request->imageid();
        std::string format = request->format();
        int paramsCnt = request->params_size();
        std::vector<int> params(paramsCnt);
        for (int i = 0; i < paramsCnt; ++i) {
            params[i] = request->params(i);
        }
        int expectedW = request->expectedw();
        int expectedH = request->expectedh();
        response->set_imageid(0);
        auto imageLoaderController = ImageLoaderController::getSingletonInstance();
        auto imgLoader = imageLoaderController->getImageLoader(connectId);
        if (nullptr == imgLoader) {
            throw  std::runtime_error("The imgLoader is nullptr. Unable to load the image.\n");
        }
        imageLoaderController->startUsingLoader(connectId);
        auto imageInfo = imgLoader->getImgById(imageId);
        if (0 == imageInfo.imageId || imageInfo.image.empty()) {
            imageLoaderController->stopUsingLoader(connectId);
            throw  std::runtime_error("Image is NULL.\n");
        }
        imageLoaderController->stopUsingLoader(connectId);
        if (0 != imageInfo.imageId) {
            response->set_imageid(imageInfo.imageId);
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
            response->set_h(imageInfo.image.rows);
            response->set_w(imageInfo.image.cols);
            // 无参数时，默认使用 .jpg 无损压缩
            if (0 == format.size()) {
                format = ".jpg";
                params = {cv::IMWRITE_PNG_COMPRESSION, 100};
            }
            std::vector<uchar> buf;
            // TODO: 在这里压缩图像会有一些性能冗余
            cv::imencode(format, imageInfo.image, buf, params);
            size_t bufSize = buf.size();
            response->set_buf(&buf[0], buf.size());
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
        int64_t imageId = request->imageid();
        std::string format = request->format();
        bool onlyImageId = request->onlyimageid();
        int paramsCnt = request->params_size();
        std::vector<int> params(paramsCnt);
        for (int i = 0; i < paramsCnt; ++i) {
            params[i] = request->params(i);
        }
        int expectedW = request->expectedw();
        int expectedH = request->expectedh();
        response->set_imageid(0);        

        auto imageLoaderController = ImageLoaderController::getSingletonInstance();
        auto imgLoader = imageLoaderController->getImageLoader(connectId);
        if (nullptr == imgLoader) {
            responseCode = 400;
            responseMessage += "The imgLoader is nullptr. Unable to load the image.\n";
        }
        
        imageLoaderController->startUsingLoader(connectId);
        if (0 == imageId && !imgLoader->hasNext()) {
            imageLoaderController->stopUsingLoader(connectId);
            responseCode = 400;
            responseMessage += "No more images available.\n";
        }
        
        else {
            auto imageInfo = imgLoader->next(imageId);
            imageLoaderController->stopUsingLoader(connectId);
            if (0 == imageInfo.imageId || imageInfo.image.empty()) {
                throw  std::runtime_error("Image is NULL.\n");
            }
            response->set_imageid(imageInfo.imageId);
            if (!onlyImageId) {
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
                response->set_h(imageInfo.image.rows);
                response->set_w(imageInfo.image.cols);
                // 无参数时，默认使用 .jpg 无损压缩
                if (0 == format.size()) {
                    format = ".jpg";
                    params = {cv::IMWRITE_PNG_COMPRESSION, 100};
                }
                std::vector<uchar> buf;
                // TODO: 在这里压缩图像会有一些性能冗余
                cv::imencode(format, imageInfo.image, buf, params);
                size_t bufSize = buf.size();
                response->set_buf(&buf[0], buf.size());
            }
        }
    } catch (const std::exception& e) {
        responseCode = 400;
        responseMessage += e.what();
    }
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}
