/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     webcamera_hikvision_sdk.h                                       *
*  @brief    The Hikvision camera image loader made using HCNetSDK           *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :HCNetSDK                                                         *
*****************************************************************************/

#ifndef _WEBCAMERA_HIKVISION_H_
#define _WEBCAMERA_HIKVISION_H_

#include "image_loaders/image_loader_base.h"
#include <unordered_map>

class HikvisionVideoReader : public ImageLoaderBase {
public:
    HikvisionVideoReader();
    virtual ~HikvisionVideoReader() override;

    bool setArgument(std::string, std::string) override;
    virtual bool start() override;
    virtual bool hasNext() override;
    virtual ImageInfo next(int64_t previousImageId = 0) override;
    virtual ImageInfo getImageById(int64_t imageId) override;
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
    HikvisionVideoReader::VideoBufInfo* videoBufInfo;
    std::unordered_map<std::string, std::string> args;
};

#endif /* _WEBCAMERA_HIKVISION_H_ */
