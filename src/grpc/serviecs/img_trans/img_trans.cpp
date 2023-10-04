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
    // TODO: 该映射表的key未来考虑通过grpc发送出去
    static std::unordered_map<std::string, ImageLoaderFactory::SourceType> sourceTypeMap = {
        {"hikvision", ImageLoaderFactory::SourceType::WebCameraHikvision},
        {"local image", ImageLoaderFactory::SourceType::LocalImage}
    };
    response->set_connectid(-1);
    auto imgType = sourceTypeMap.find(request->imgtype());
    if (sourceTypeMap.end() != imgType) {
        int sourceCnt = request->sources_size();
        std::vector<std::string> sources(sourceCnt);
        for (int i = 0; i < sourceCnt; ++i) {
            sources[i] = request->sources(i);
        }
        auto imageLoaderController = ImageLoaderController::getSingletonInstance();
        int64_t connectId = imageLoaderController->registerImageLoader(sources, imgType->second);
        response->set_connectid(connectId);
    }
    return grpc::Status::OK;
}

grpc::Status ImgTransService::getImg(grpc::ServerContext *context, const imgTrans::GetImgRequest *request, imgTrans::GetImgResponse *response)
{
    int64_t connectId = request->connectid();
    auto imageLoaderController = ImageLoaderController::getSingletonInstance();
    auto imgLoader = imageLoaderController->getImageLoader(connectId);
    response->set_imgid(-1);
    if (imgLoader) {
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
