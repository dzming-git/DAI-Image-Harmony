/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     local_image_loader.h                                            *
*  @brief    Local Image Loader                                              *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :                                                                 *
*****************************************************************************/

#ifndef _WEBCAMERA_HIKVISION_H_
#define _WEBCAMERA_HIKVISION_H_

#include "image_loaders/image_loader_base.h"
#include <unordered_map>

class WebcameraHikvisionLoader : public ImageLoaderBase {
public:
    WebcameraHikvisionLoader();
    virtual ~WebcameraHikvisionLoader() override;

    bool setArgument(std::string, std::string) override;
    virtual bool start() override;
    virtual bool isUnique() override;
    virtual bool hasNext() override;
    virtual ImageInfo next(int64_t previousImageId = 0) override;
    virtual ImageInfo getImgById(int64_t imageId) override;
    virtual size_t getTotalCount() override;
    virtual size_t getCurrentIndex() override;
    class VideoBufInfo;

private:
    size_t totalCnt;
    size_t currIdx;
    bool playOk;
    bool initOk;
    long userId;
    int nPort;
    long handle;
    WebcameraHikvisionLoader::VideoBufInfo* videoBufInfo;
    std::unordered_map<std::string, std::string> args;
};

class WebcameraHikvisionLoader::VideoBufInfo {
public:
    VideoBufInfo();
    ~VideoBufInfo();

    int h;
    int w;
    int bufLen;
    bool updated;
    char* bufShallowcopy;
    int historyMaxSize;  // 内存池最多缓存多少图片
    char* historyFrameMemoryPool;  // 内存池
    int historyFrameMemoryPoolUpdateIndex;  // 当前需要写入的位置
    std::unordered_map<int64_t, char*> history;  // 通过时间戳查询图片指针的哈希表
    std::queue<int64_t> historyOrder;  // 按照顺序记录时间戳，目的是缓存满时删除最早图片
    pthread_mutex_t historyLock;
};

#endif /* _WEBCAMERA_HIKVISION_H_ */
