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
    enum SourceType : uint8_t;
    static ImageLoaderBase* createImageLoader(ImageLoaderFactory::SourceType type);
};

#endif /* _IMAGE_LOADER_FACTORY_H_ */