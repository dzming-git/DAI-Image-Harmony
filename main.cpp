#if 0
#include "http_server/http_server.h"
#include "http_server/http_server_builder.h"

int main() {
    auto builder = HttpServer::Builder();
    builder.setHost("0.0.0.0");
    builder.setPort(5000);
    auto server = builder.build();
    server->start();
}

#else

#include <iostream>
#include <string>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "protos/img_trans/img_trans.grpc.pb.h"
#include "protos/img_trans/img_trans.pb.h"
#include "image_loaders/image_loader_factory.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>

#include "grpc/grpc_server.h"
#include "grpc/grpc_server_builder.h"

using grpc::ServerContext;
using grpc::Status;
using imgTrans::ImgMsg;
using imgTrans::NullMsg;

ImageLoaderBase* imgLoader_ = ImageLoaderFactory::createImageLoader(ImageLoaderFactory::SourceType::WebCameraHikvision);

class imgTransServiceImpl : public imgTrans::Communicate::Service {
    virtual Status getImg(ServerContext* context, const NullMsg* request, ImgMsg* response) override {      
        auto imgBGR = imgLoader_->next();
        response->set_h(imgBGR.rows);
        response->set_w(imgBGR.cols);
        response->set_buf(imgBGR.data, 3 * imgBGR.rows * imgBGR.cols);
        return Status::OK;
    }
};

int main(int argc, char** argv) {
    std::ifstream file("/workspace/source.txt"); // 打开文件
    std::string ip, username, password, port;
    if (file.is_open()) { // 确保文件成功打开
        std::string line;
        while (std::getline(file, line)) { // 逐行读取文件内容
            std::stringstream ss(line); // 创建字符串流对象，并传入当前行的内容
            ss >> ip >> username >> password >> port; // 按照空格分割字符串，并依次读入变量
        }
        file.close(); // 关闭文件
    } else {
        std::cout << "无法打开文件" << std::endl;
    }
    imgLoader_->setSource({ip, username, password, port});
    GRPCServer::GRPCServerBuilder builder;
    grpc::Service* service = new imgTransServiceImpl();
    builder.setHost("0.0.0.0")
           .setEpollCount(4, 8)
           .setMaxSendBytes(1024 * 1024 * 1024)
           .addService(service);
    auto server = builder.build();
    server->start();
    delete service;
    return 0;
}

#endif