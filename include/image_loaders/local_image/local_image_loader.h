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

#ifndef _LOCAL_IMAGE_LOADER_H_
#define _LOCAL_IMAGE_LOADER_H_

#include "image_loader_base.h"

class LocalImageLoader : public ImageLoaderBase {
public:
    LocalImageLoader();
    void setSource(std::string path) override;
    virtual bool hasNext() override;
    virtual cv::Mat next() override;
    virtual size_t getTotalCount() override;
    virtual size_t getCurrentIndex() override;
    virtual ~LocalImageLoader() override;
private:
    std::string source;
    cv::Mat img;
    size_t totalCnt;
    size_t currIdx;
};

#endif /* _LOCAL_IMAGE_LOADER_H_ */