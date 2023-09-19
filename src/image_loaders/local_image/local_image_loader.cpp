#include "local_image_loader.h"

LocalImageLoader::LocalImageLoader(): totalCnt(0), currIdx(0) {
}

void LocalImageLoader::setSource(std::initializer_list<std::string> paths) {
    this->paths = paths;
    totalCnt = this->paths.size();
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

LocalImageLoader::~LocalImageLoader() {
}
