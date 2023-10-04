/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     image_loader_controller.h                                       *
*  @brief    Using RAII Method to Control the Establishment and Release of   *
*            Image Loaders                                                   *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :                                                                 *
*****************************************************************************/

#ifndef _IMAGE_LOADER_CONTRILLER_H_
#define _IMAGE_LOADER_CONTRILLER_H_

#include <atomic>
#include <mutex>
#include <unordered_map>
#include "image_loaders/image_loader_base.h"
#include "image_loaders/image_loader_factory.h"

class ImageLoaderController {
public:
    struct ImageLoaderPtrAndCount {
        ImageLoaderBase* ptr;
        int cnt;
    };
    static ImageLoaderController* getSingletonInstance();
    ImageLoaderBase* getImageLoader(int64_t);
    int64_t registerImageLoader(std::vector<std::string>, ImageLoaderFactory::SourceType);
    void unregisterImageLoader(int64_t);
private:
    ImageLoaderController();
    static ImageLoaderController* instance;
    static pthread_mutex_t lock;

    std::unordered_map<int64_t, ImageLoaderPtrAndCount> loadersMap;
};

#endif /* _IMAGE_LOADER_CONTRILLER_H_ */
