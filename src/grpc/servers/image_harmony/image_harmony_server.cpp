#include "grpc/servers/image_harmony/image_harmony_server.h"
#include "image_loaders/image_loader_factory.h"
#include "image_loaders/image_loader_controller.h"
#include <chrono>
#include <random>
#include <vector>
#include "config/config.h"

ImageHarmonyServer::ImageHarmonyServer() {
    auto config = Config::getSingletonInstance();
    historyMaxSize = config->getHistoryMaxSize();
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

grpc::Status ImageHarmonyServer::getImg(grpc::ServerContext *context, const imageHarmony::GetImgRequest *request, imageHarmony::GetImgResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    int64_t connectId = request->connectid();
    int64_t imgId = request->imgid();
    std::string format = request->format();
    int paramsCnt = request->params_size();
    std::vector<int> params(paramsCnt);
    for (int i = 0; i < paramsCnt; ++i) {
        params[i] = request->params(i);
    }
    int expectedW = request->expectedw();
    int expectedH = request->expectedh();
    response->set_imgid(-1);        
    // TODO: 临时用本地生成的时间戳，未来再接入时间同步器
    auto now = std::chrono::system_clock::now();
    auto timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    cv::Mat imgBGR;
    // 获取最新图像
    if (0 == imgId) {
        auto imageLoaderController = ImageLoaderController::getSingletonInstance();
        auto imgLoader = imageLoaderController->getImageLoader(connectId);
        if (nullptr == imgLoader) {
            responseCode = 400;
            responseMessage += "The imgLoader is nullptr. Unable to load the image.\n";
        } else if (!imgLoader->hasNext()) {
            responseCode = 400;
            responseMessage += "No more images available.\n";
        }
        
        else {
            response->set_imgid(timeStamp);
            pthread_mutex_lock(&historyLock);
            if (historyOrder.size() >= historyMaxSize) {
                int64_t removeImgId = historyOrder.front();
                history.erase(removeImgId);
                historyOrder.pop();
            }
            historyOrder.emplace(timeStamp);
            history[timeStamp] = imgLoader->next();
            // 深拷贝，避免后续的resize影响原始图像
            imgBGR = history[timeStamp].clone();
            pthread_mutex_unlock(&historyLock);
        }
    }
    // 调取历史图像
    else {
        pthread_mutex_lock(&historyLock);
        auto it = history.find(imgId);
        if (it == history.end()) {
            responseCode = 400;
            responseMessage += "img ID not found.\n";
        }
        else {
            imgBGR = it->second;
        }
        pthread_mutex_unlock(&historyLock);
    }

    if (!imgBGR.empty()) {
        if (expectedW * expectedH && !(expectedW == imgBGR.cols && expectedH == imgBGR.rows)) {
            double aspectRatio = (double)imgBGR.cols / imgBGR.rows;
            double expectedAspectRatio = (double)expectedW / expectedH;
            if (abs(aspectRatio - expectedAspectRatio) > 0.01) {
                std::string originalAspectRatio = std::to_string(aspectRatio);
                std::string currentAspectRatio = std::to_string(expectedAspectRatio);
                responseMessage += "The aspect ratio has changed. Original aspect ratio: " + originalAspectRatio + ". Current aspect ratio: " + currentAspectRatio + ".\n";
            }
            cv::resize(imgBGR, imgBGR, cv::Size(expectedW, expectedH));
        }
        response->set_h(imgBGR.rows);
        response->set_w(imgBGR.cols);
        // 无参数时，默认使用 .jpg 无损压缩
        if (0 == format.size()) {
            format = ".jpg";
            params = {cv::IMWRITE_PNG_COMPRESSION, 100};
        }
        std::vector<uchar> buf;
        // TODO: 在这里压缩图像会有一些性能冗余
        cv::imencode(format, imgBGR, buf, params);
        size_t bufSize = buf.size();
        response->set_buf(&buf[0], buf.size());
    }
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}
