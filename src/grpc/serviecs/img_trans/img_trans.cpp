#include "grpc/serviecs/img_trans/img_trans.h"
#include "image_loaders/image_loader_factory.h"
#include <chrono>
#include <random>
#include <vector>

ImgTransService::ImgTransService() {
}

ImgTransService::~ImgTransService() {
}

int64_t generateRandomInt64() {
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<int64_t> distribution(
        std::numeric_limits<int64_t>::min(),
        std::numeric_limits<int64_t>::max()
    );
    return distribution(generator);
}

grpc::Status ImgTransService::registerImgTransService(grpc::ServerContext*, const imgTrans::RegisterImgTransServiceRequest *request, imgTrans::RegisterImgTransServiceResponse *response) {
    static std::unordered_map<std::string, ImageLoaderFactory::SourceType> sourceTypeMap = {
        {"hikvision", ImageLoaderFactory::SourceType::WebCameraHikvision},
        {"local image", ImageLoaderFactory::SourceType::LocalImage}
    };
    response->set_connectid(-1);
    auto imgType = sourceTypeMap.find(request->imgtype());
    if (sourceTypeMap.end() != imgType) {
        ImageLoaderBase* imgLoader = ImageLoaderFactory::createImageLoader(imgType->second);
        int sourceCnt = request->sources_size();
        std::vector<std::string> sources(sourceCnt);
        for (int i = 0; i < sourceCnt; ++i) {
            sources[i] = request->sources(i);
        }
        if (nullptr != imgLoader) {
            imgLoader->setSource(sources);
            // TODO: 临时使用本地随机数生成器，未来再接入分布式唯一ID生成器
            int64_t connectId = generateRandomInt64();
            imgLoaders.emplace(connectId, imgLoader);
            response->set_connectid(connectId);
        }
        
    }
    return grpc::Status::OK;
}

grpc::Status ImgTransService::getImg(grpc::ServerContext *context, const imgTrans::GetImgRequest *request, imgTrans::GetImgResponse *response)
{
    int64_t connectId = request->connectid();
    auto imgLoader = imgLoaders.find(connectId);
    response->set_imgid(-1);
    if (imgLoaders.end() != imgLoader) {
        auto imgBGR = imgLoaders[connectId]->next();
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
