/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     image_loader_facotry.h                                          *
*  @brief    Image Loader Factory                                            *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :                                                                 *
*****************************************************************************/

#ifndef _IMAGE_LOADER_FACTORY_H_
#define _IMAGE_LOADER_FACTORY_H_

#include "image_loaders/image_loader_base.h"

class ImageLoaderFactory {
public:
    enum class SourceType {
        LocalImage,
        LocalVideo,
        WebImage,
        WebVideo,
        WebCameraHikvision,
        GRPCImage,
        GRPCVideo,
    };
    static ImageLoaderBase* createImageLoader(ImageLoaderFactory::SourceType);
};

#endif /* _IMAGE_LOADER_FACTORY_H_ */
