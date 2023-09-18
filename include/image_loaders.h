/*****************************************************************************
*  Copyright © 2023 - 2023 dzming  dzm_work@163.com.                         *
*                                                                            *
*  @file     image_loaders.h                                                 *
*  @brief    图像加载器                                                        *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  : 使用简单工厂模式、迭代器模式、适配器模式实现多源图像数据的统一输出         *
*****************************************************************************/

#ifndef _IMAGE_LOADERS_H_
#define _IMAGE_LOADERS_H_

#include <opencv2/opencv.hpp>
#include <string>

class ImageLoaderBase {
public:
    virtual void setSource(std::string path) = 0;
    virtual bool hasNext() = 0;
    virtual cv::Mat next() = 0;
    virtual size_t getTotalCount() = 0;
    virtual size_t getCurrentIndex() = 0;
    virtual ~ImageLoaderBase();
};

class ImageLoaderFactory {
public:
    enum SourceType {
        LocalImage
    };
    static ImageLoaderBase* createImageLoader(ImageLoaderFactory::SourceType type);
};

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

#endif /* _IMAGE_LOADERS_H_ */
