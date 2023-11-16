/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     image_harmony_server.h                                          *
*  @brief    gRPC impl: image harmony                                        *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :  proto file: resources/protos/image_harmony.proto               *
*****************************************************************************/

#ifndef _IMAGE_HARMONY_SERVER_H_
#define _IMAGE_HARMONY_SERVER_H_

#include <string>
#include "protos/image_harmony/image_harmony.grpc.pb.h"
#include "protos/image_harmony/image_harmony.pb.h"
#include "grpc/servers/grpc_server.h"
#include "grpc/servers/grpc_server_builder.h"
#include "image_loaders/image_loader_base.h"

class ImageHarmonyServer: public imageHarmony::Communicate::Service {
public:
    ImageHarmonyServer();
    virtual ~ImageHarmonyServer();

    virtual grpc::Status registerImgTransService(grpc::ServerContext*, const imageHarmony::RegisterImgTransServiceRequest*, imageHarmony::RegisterImgTransServiceResponse*) override;
    virtual grpc::Status unregisterImgTransService(grpc::ServerContext*, const imageHarmony::UnregisterImgTransServiceRequest*, imageHarmony::UnregisterImgTransServiceResponse*) override;
    virtual grpc::Status getImg(grpc::ServerContext*, const imageHarmony::GetImgRequest*, imageHarmony::GetImgResponse*) override;
private:
    std::unordered_map<int64_t, cv::Mat> history;
    std::queue<int64_t> historyOrder;
    int historyMaxSize;
    pthread_mutex_t historyLock;
};

#endif /* _IMAGE_HARMONY_SERVER_H_ */
