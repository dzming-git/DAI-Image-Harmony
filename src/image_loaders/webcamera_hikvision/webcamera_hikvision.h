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
    virtual cv::Mat next() override;
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
    char* bufShallowcopy;
    char* bufDeepcopy;
};

#endif /* _WEBCAMERA_HIKVISION_H_ */
