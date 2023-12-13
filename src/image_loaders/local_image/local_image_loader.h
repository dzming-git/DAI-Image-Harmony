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

#include "image_loaders/image_loader_base.h"
#include <vector>

class LocalImageLoader : public ImageLoaderBase {
public:
    LocalImageLoader();
    bool setArgument(std::string, std::string) override;
    virtual bool start() override;
    virtual bool isUnique() override;
    virtual bool hasNext() override;
    virtual ImageInfo next(int64_t previousImageId = 0) override;
    virtual ImageInfo getImageById(int64_t imageId) override;
    virtual size_t getTotalCount() override;
    virtual size_t getCurrentIndex() override;
    virtual ~LocalImageLoader() override;
private:
    std::vector<int64_t> hashs;
    std::unordered_map<int64_t, std::string> filePathMap;
    std::unordered_map<int64_t, int64_t> nextMap;
    size_t totalCnt;
    size_t currIdx;
};

#endif /* _LOCAL_IMAGE_LOADER_H_ */