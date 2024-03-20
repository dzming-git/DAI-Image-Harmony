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

    virtual grpc::Status initImageTransService(grpc::ServerContext*, const imageHarmony::InitImageTransServiceRequest*, imageHarmony::InitImageTransServiceResponse*) override;
    virtual grpc::Status connectImageTransService(grpc::ServerContext*, const imageHarmony::ConnectImageTransServiceRequest*, imageHarmony::ConnectImageTransServiceResponse*) override;
    virtual grpc::Status disconnectImageTransService(grpc::ServerContext*, const imageHarmony::DisconnectImageTransServiceRequest*, imageHarmony::DisconnectImageTransServiceResponse*) override;
    virtual grpc::Status getImageByImageId(grpc::ServerContext*, const imageHarmony::GetImageByImageIdRequest*, imageHarmony::GetImageByImageIdResponse*) override;
    virtual grpc::Status getNextImageByImageId(grpc::ServerContext*, const imageHarmony::GetNextImageByImageIdRequest*, imageHarmony::GetNextImageByImageIdResponse*) override;
private:
};

#endif /* _IMAGE_HARMONY_SERVER_H_ */
