#include "local_image_loader.h"

LocalImageLoader::LocalImageLoader(): totalCnt(0), currIdx(0) {
}

void LocalImageLoader::setSource(std::string path) {
    source = path;
    img = cv::imread(source);
    if (!img.empty()) {
        totalCnt = 1;
    }
}

bool LocalImageLoader::hasNext() {
    return currIdx < totalCnt;
}

cv::Mat LocalImageLoader::next() {
    ++currIdx;
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
