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

cv::Mat LocalImageLoader::next() {
    img.release();
    while (hasNext() && img.empty()) {
        img = cv::imread(this->paths[currIdx++]);
    }
    return img;
}

size_t LocalImageLoader::getTotalCount() {
    return totalCnt;
}

size_t LocalImageLoader::getCurrentIndex() {
    return currIdx;
}
