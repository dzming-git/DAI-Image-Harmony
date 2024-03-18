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

#ifndef _WEBCAMERA_HIKVISION_OPENCV_CPU_H_
#define _WEBCAMERA_HIKVISION_OPENCV_CPU_H_

#include "image_loaders/image_loader_base.h"
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>

class OpencvVideoReader_CPU : public ImageLoaderBase {
public:
    OpencvVideoReader_CPU();
    virtual ~OpencvVideoReader_CPU() override;

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
    OpencvVideoReader_CPU::VideoBufInfo* videoBufInfo;
    std::unordered_map<std::string, std::string> args;

    static void videoReadThreadFunc(OpencvVideoReader_CPU::VideoBufInfo* videoBufInfo, bool* videoReadThreadStop);
    bool videoReadThreadStop;
    std::thread videoReadThread;
    std::mutex mtx;
    std::condition_variable cv;
};

#endif /* _WEBCAMERA_HIKVISION_OPENCV_CPU_H_ */
