/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     image_loader_base.h                                             *
*  @brief    Abstract Image Loader                                           *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :                                                                 *
*****************************************************************************/

#ifndef _IMAGE_LOADER_BASE_H_
#define _IMAGE_LOADER_BASE_H_

#include <opencv2/opencv.hpp>
#include <string>

class ImageLoaderBase {
public:
    virtual bool setSource(std::initializer_list<std::string>) = 0;
    virtual bool hasNext() = 0;
    virtual cv::Mat next() = 0;
    virtual size_t getTotalCount() = 0;
    virtual size_t getCurrentIndex() = 0;
    virtual ~ImageLoaderBase();
};

#endif /* _IMAGE_LOADER_BASE_H_ */