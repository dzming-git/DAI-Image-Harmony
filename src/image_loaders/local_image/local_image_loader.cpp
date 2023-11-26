#include "image_loaders/local_image/local_image_loader.h"
#include <iostream>
#include <sstream>

LocalImageLoader::LocalImageLoader(): totalCnt(0), currIdx(0) {
    std::cout << "Create LocalImageLoader" << std::endl;
}

LocalImageLoader::~LocalImageLoader() {
    std::cout << "Destroy LocalImageLoader" << std::endl;
}

bool LocalImageLoader::setArgument(std::string key, std::string value) {
    if ("ImagePaths" == key) {
        std::stringstream ss(value);
        std::string path;
        while (getline(ss, path, '\n')) {
            this->paths.emplace_back(path);
        }
        return true;
    }
    return false;
}

bool LocalImageLoader::start() {
    totalCnt = this->paths.size();
    return true;
}

bool LocalImageLoader::isUnique() {
    return true;
}

bool LocalImageLoader::hasNext() {
    return currIdx < totalCnt;
}

ImageInfo LocalImageLoader::next(int64_t previousImageId) {
    ImageInfo imageInfo;
    // TODO 暂时没有开发previousImageId相关的功能
    img.release();
    if (hasNext() && img.empty()) {
        img = cv::imread(this->paths[currIdx++]);
        imageInfo.image = img;
        // TODO 临时代码
        imageInfo.imageId = rand();
    }
    return imageInfo;
}

ImageInfo LocalImageLoader::getImgById(int64_t imageId) {
    // TODO 未开发
    ImageInfo imageInfo;
    return imageInfo;
}

size_t LocalImageLoader::getTotalCount() {
    return totalCnt;
}

size_t LocalImageLoader::getCurrentIndex() {
    return currIdx;
}
