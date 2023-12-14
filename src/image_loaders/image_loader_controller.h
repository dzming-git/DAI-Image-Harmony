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
        int64_t loaderArgsHash = 0;
        std::chrono::steady_clock::time_point lastRequestTime;
        int userCnt; // 使用者数量
        pthread_mutex_t userCntLock;
        void updateTime();
    };
    static ImageLoaderController* getSingletonInstance();
    ImageLoaderBase* getImageLoader(int64_t connectionId);
    bool startUsingLoader(int64_t connectionId);
    bool stopUsingLoader(int64_t connectionId);
    bool registerImageLoader(std::unordered_map<std::string, std::string>, ImageLoaderFactory::SourceType, int64_t &loaderArgsHash, int64_t &connectionId, bool isUnique);
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
