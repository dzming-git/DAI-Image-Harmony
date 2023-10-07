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
    response->set_connectid(-1);
    auto imgType = ImageLoaderFactory::sourceTypeMap.find(request->imgtype());
    if (ImageLoaderFactory::sourceTypeMap.end() != imgType) {
        int argsCnt = request->args_size();
        std::vector<std::pair<std::string, std::string>> args(argsCnt);
        for (int i = 0; i < argsCnt; ++i) {
            auto arg = request->args(i);
            args[i].first = arg.key();
            args[i].second = arg.value();
        }
        auto imageLoaderController = ImageLoaderController::getSingletonInstance();
        int64_t connectId = imageLoaderController->registerImageLoader(args, imgType->second);
        response->set_connectid(connectId);
    }
    return grpc::Status::OK;
}

grpc::Status ImgTransService::unregisterImgTransService(grpc::ServerContext *context, const imgTrans::UnregisterImgTransServiceRequest *request, imgTrans::UnregisterImgTransServiceResponse *response) {
    int64_t connectId = request->connectid();
    auto imageLoaderController = ImageLoaderController::getSingletonInstance();
    imageLoaderController->unregisterImageLoader(connectId);
    return grpc::Status::OK;
}
grpc::Status ImgTransService::getImg(grpc::ServerContext *context, const imgTrans::GetImgRequest *request, imgTrans::GetImgResponse *response)
{
    int64_t connectId = request->connectid();
    auto imageLoaderController = ImageLoaderController::getSingletonInstance();
    auto imgLoader = imageLoaderController->getImageLoader(connectId);
    response->set_imgid(-1);
    if (imgLoader && imgLoader->hasNext()) {
        auto imgBGR = imgLoader->next();
        response->set_h(imgBGR.rows);
        response->set_w(imgBGR.cols);
        // TODO: 临时用本地生成的时间戳，未来再接入时间同步器
        auto now = std::chrono::system_clock::now();
        auto timeStamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        response->set_imgid(timeStamp);
        response->set_buf(imgBGR.data, 3 * imgBGR.rows * imgBGR.cols);
    }
    return grpc::Status::OK;
}
