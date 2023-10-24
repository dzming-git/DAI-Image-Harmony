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

#include <mutex>
#include <unordered_map>
#include <chrono>
#include <thread>
#include "image_loaders/image_loader_base.h"
#include "image_loaders/image_loader_factory.h"

class ImageLoaderController {
public:
    struct ImageLoaderInfo {
        ImageLoaderBase* ptr = nullptr;
        int cnt = 0;
    };
    struct ConnectionInfo {
        int64_t loaderArgsHash = -1;
        std::chrono::steady_clock::time_point lastRequestTime;
        void updateTime();
    };
    static ImageLoaderController* getSingletonInstance();
    ImageLoaderBase* getImageLoader(int64_t connectionId);
    int64_t registerImageLoader(std::vector<std::pair<std::string, std::string>>, ImageLoaderFactory::SourceType);
    bool unregisterImageLoader(int64_t);
    void setConnectionTimeout(int timeout);
private:
    ImageLoaderController();
    void checkConnections();
    void startCheckConnections();
    int connectionTimeout;
    std::thread checkConnectionsThread;

    static ImageLoaderController* instance;
    static pthread_mutex_t lock;
    

    std::unordered_map<int64_t /* loaderArgsHash */, ImageLoaderInfo> loadersMap;
    std::unordered_map<int64_t /* connectionId */, ConnectionInfo> connectionsMap;
};

#endif /* _IMAGE_LOADER_CONTRILLER_H_ */