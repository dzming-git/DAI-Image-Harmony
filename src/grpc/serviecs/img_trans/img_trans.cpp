#include "grpc/serviecs/img_trans/img_trans.h"
#include "image_loaders/image_loader_factory.h"
#include "image_loaders/image_loader_controller.h"
#include <chrono>
#include <random>
#include <vector>

ImgTransService::ImgTransService() {
}

ImgTransService::~ImgTransService() {
}

grpc::Status ImgTransService::registerImgTransService(grpc::ServerContext*, const imgTrans::RegisterImgTransServiceRequest *request, imgTrans::RegisterImgTransServiceResponse *response) {
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
    int argsCnt = request->args_size();
    std::vector<std::pair<std::string, std::string>> args(argsCnt);
    for (int i = 0; i < argsCnt; ++i) {
        auto arg = request->args(i);
        args[i].first = arg.key();
        args[i].second = arg.value();
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

grpc::Status ImgTransService::unregisterImgTransService(grpc::ServerContext *context, const imgTrans::UnregisterImgTransServiceRequest *request, imgTrans::UnregisterImgTransServiceResponse *response) {
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
grpc::Status ImgTransService::getImg(grpc::ServerContext *context, const imgTrans::GetImgRequest *request, imgTrans::GetImgResponse *response) {
    int32_t responseCode = 200;
    std::string responseMessage;
    int64_t connectId = request->connectid();
    std::string format = request->format();
    int paramsCnt = request->params_size();
    std::vector<int> params(paramsCnt);
    for (int i = 0; i < paramsCnt; ++i) {
        params[i] = request->params(i);
    }
    response->set_imgid(-1);
    auto imageLoaderController = ImageLoaderController::getSingletonInstance();
    auto imgLoader = imageLoaderController->getImageLoader(connectId);
    if (nullptr == imgLoader) {
    responseCode = 400;
    responseMessage = "The imgLoader is nullptr. Unable to load the image.\n";
    } else if (!imgLoader->hasNext()) {
    responseCode = 400;
    responseMessage = "No more images available.\n";
    }
    else {
        auto imgBGR = imgLoader->next();
        response->set_h(imgBGR.rows);
        response->set_w(imgBGR.cols);
        // TODO: 临时用本地生成的时间戳，未来再接入时间同步器
        auto now = std::chrono::system_clock::now();
        auto timeStamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        response->set_imgid(timeStamp);
        if (format.size()) {
            std::vector<uchar> buf;
            // TODO: 在这里压缩图像会有一些性能冗余
            cv::imencode(format, imgBGR, buf, params);
            size_t bufSize = buf.size();
            response->set_buf(&buf[0], buf.size());
        }
        else {
            response->set_buf(imgBGR.data, 3 * imgBGR.rows * imgBGR.cols);
        }
    }
    response->mutable_response()->set_code(responseCode);
    response->mutable_response()->set_message(responseMessage);
    return grpc::Status::OK;
}
