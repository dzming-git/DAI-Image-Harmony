/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     img_trans.h                                                     *
*  @brief    gRPC impl: img trans                                            *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :  proto file: resources/protos/img_trans.proto                   *
*****************************************************************************/

#ifndef _IMG_TRANS_H_
#define _IMG_TRANS_H_

#include <string>
#include "protos/img_trans/img_trans.grpc.pb.h"
#include "protos/img_trans/img_trans.pb.h"
#include "grpc/grpc_server.h"
#include "grpc/grpc_server_builder.h"
#include "image_loaders/image_loader_base.h"

class ImgTransService: public imgTrans::Communicate::Service {
public:
    ImgTransService();
    virtual ~ImgTransService();

    virtual grpc::Status registerImgTransService(grpc::ServerContext*, const imgTrans::RegisterImgTransServiceRequest*, imgTrans::RegisterImgTransServiceResponse*) override;
    virtual grpc::Status unregisterImgTransService(grpc::ServerContext*, const imgTrans::UnregisterImgTransServiceRequest*, imgTrans::UnregisterImgTransServiceResponse*) override;
    virtual grpc::Status getImg(grpc::ServerContext*, const imgTrans::GetImgRequest*, imgTrans::GetImgResponse*) override;
private:
};

#endif /* _IMG_TRANS_H_ */
