#include "http_server/http_server.h"
#include "http_server/http_server_builder.h"
#include "image_loaders/image_loader_factory.h"
#include "image_loaders/image_loader_controller.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include "hv/hthread.h"
#include "hv/hasync.h"
#include "hv/hlog.h"
#include "hv/HttpContext.h"

std::string HttpServer::getHost() {
    return host;
}

uint16_t HttpServer::getPort() {
    return port;
}

HttpServer::~HttpServer() {
}

HttpServer::HttpServer(HttpServer::Builder *builder): host(builder->getHost()), port(builder->getPort()) {
    // HV_MEMCHECK;

    // route
    router.GET("/video", [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        try {
            std::string sourceTypeStr = "";
            bool isUnique = false;
            std::unordered_map<std::string, std::string> args;
            auto params = req->query_params;
            for (auto param : params) {
                std::string key = param.first;
                std::string value = param.second;
                if ("source_type" == key) {
                    sourceTypeStr = value;
                }
                else if ("is_unique" == key) {
                    isUnique = value != "0";
                }
                else {
                    args.emplace(key, value);
                }
            }
            if ("" == sourceTypeStr) {
                throw std::runtime_error("source_type not set.\n");
            }
            ImageLoaderFactory::SourceType sourceType;
            auto sourceTypeIt = ImageLoaderFactory::sourceTypeMap.find(sourceTypeStr);
            if (ImageLoaderFactory::sourceTypeMap.end() == sourceTypeIt) {
                // 匹配不到，用编辑距离智能匹配
                std::string mostSimilarType = ImageLoaderFactory::getMostSimilarSourceType(sourceTypeStr);
                sourceType = ImageLoaderFactory::sourceTypeMap[mostSimilarType];
            }
            else {
                sourceType = sourceTypeIt->second;
            }

            auto imageLoaderController = ImageLoaderController::getSingletonInstance();
            int64_t loaderArgsHash = 0;
            bool ok = imageLoaderController->initImageLoader(args, sourceType, isUnique, loaderArgsHash);
            if (!ok) {
                throw std::runtime_error("Failed to init image loader.\n");
            }
            int64_t connectionId = 0;
            ok = imageLoaderController->connectImageLoader(loaderArgsHash, connectionId);
            auto loader = imageLoaderController->getImageLoader(connectionId);
            const int targetSize = 200 * 1024;  // 目标大小200KB
            int quality = 90; // 初始压缩率
            const int minQuality = 30; // 最小压缩率
            const int step = 10; // 压缩率调整步长
            while (loader->hasNext() && writer->isConnected()) {
                writer->WriteHeader("Content-Type", "multipart/x-mixed-replace; boundary=frame");
                auto imageInfo = loader->getImageById(0);
                cv::Mat image = imageInfo.image;
                std::vector<uchar> buf;
                if (image.empty()) break;

                // 动态调整压缩率
                do {
                    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, quality};
                    if (!cv::imencode(".jpg", image, buf, params)) {
                        throw std::runtime_error("Failed to encode image.\n");
                    }
                    
                    if (buf.size() <= targetSize || quality == minQuality) {
                        break; // 如果满足大小要求或已达到最小压缩率，则退出循环
                    }
                    
                    quality -= step; // 降低压缩率
                } while (quality >= minQuality);

                std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 50};
                if (!cv::imencode(".jpg", image, buf, params)) {
                    throw std::runtime_error("Failed to encode image.\n");
                }

                // 构建MJPEG帧的HTTP响应
                std::string header = "--frame\r\nContent-Type: image/jpeg\r\n\r\n";
                std::string footer = "\r\n";
                std::string content(header);
                content.append(reinterpret_cast<char*>(buf.data()), buf.size());
                content += footer;

                // 使用异步writer发送帧
                writer->write(content.data(), content.size());
            }
        } catch (const std::exception& e) {
            writer->WriteBody(e.what());
            writer->WriteStatus(HTTP_STATUS_OK);
            writer->End();
        }
    });

    server.setHost(host.c_str());
    server.setPort(port);
    server.registerHttpService(&router);
}

void HttpServer::start() {
    try {
        server.start();
        while (1) {
            sleep(10000);
        }
    } catch (const std::exception& e) {
        hv::async::cleanup();
    }
}