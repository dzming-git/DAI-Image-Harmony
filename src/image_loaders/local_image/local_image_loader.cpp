#include "image_loaders/local_image/local_image_loader.h"

LocalImageLoader::LocalImageLoader(): totalCnt(0), currIdx(0) {
}

bool LocalImageLoader::setSource(std::vector<std::string> paths) {
    this->paths = paths;
    totalCnt = this->paths.size();
    return true;
}

bool LocalImageLoader::isUnique() {
    return true;
}

bool LocalImageLoader::hasNext()
{
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
