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

std::size_t hashCombine(std::size_t seed, const std::string& str) {
    std::hash<std::string> hasher;
    return seed ^= hasher(str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

std::int64_t hashVector(const std::vector<std::string>& vec) {
    std::int64_t hash_value = 0;
    for (const auto& str : vec) {
        hash_value = hashCombine(hash_value, str);
    }
    return hash_value;
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
        // TODO: 未来考虑分布式ID 用哈希值建立一个分布式ID映射
        int64_t connectId = hashVector(sources);
        auto imgLoaderIt = imgLoaders.find(connectId);
        ImageLoaderBase* imgLoader = nullptr;
        bool needCreateImageLoader = imgLoaders.end() == imgLoaderIt || imgLoader->isUnique();
        // 连接独占的加载器需要新建
        if (imgLoaders.end() != imgLoaderIt && imgLoader->isUnique()) {
            // 防止connectId重复
            // TODO: 未来考虑分布式ID
            do {
                connectId = generateRandomInt64();
            } while (imgLoaders.find(connectId) != imgLoaders.end());
            needCreateImageLoader = true;
        }
        if (needCreateImageLoader) {
            imgLoader = ImageLoaderFactory::createImageLoader(imgType->second);
            imgLoaders.emplace(connectId, imgLoader);
            imgLoader->setSource(sources);
        }
        response->set_connectid(connectId);
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
