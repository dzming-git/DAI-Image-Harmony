#include "image_loaders/local_image/local_image_loader.h"
#include <iostream>
#include <sstream>
#include <filesystem>

// 计算规范化后的文件路径字符串的哈希值
int64_t getNormalizedFilePathHash(const std::string& filePath) {
    std::hash<std::string> hasher;
    std::filesystem::path canonicalPath = std::filesystem::canonical(filePath);
    return static_cast<int64_t>(hasher(canonicalPath.string()));
}

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
            int64_t hash = getNormalizedFilePathHash(path);
            hashs.emplace_back(hash);
            filePathMap.emplace(hash, path);
        }
        totalCnt = hashs.size();
        for (int i = 0; i < totalCnt - 1; ++i) {
            nextMap[hashs[i]] = nextMap[hashs[i + 1]];
        }
        return true;
    }
    return false;
}

bool LocalImageLoader::start() {
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
    int64_t imageId = 0;
    if (0 == previousImageId) {
        if (hasNext()) {
            imageId = hashs[currIdx++];
        }
    }
    else if (nextMap.find(previousImageId) != nextMap.end()) {
        imageId = nextMap[previousImageId];
    }
    if (imageId) {
        imageInfo.image = cv::imread(filePathMap[imageId]);
        imageInfo.imageId = imageId;
    }
    return imageInfo;
}

ImageInfo LocalImageLoader::getImgById(int64_t imageId) {
    ImageInfo imageInfo;
    if (filePathMap.find(imageId) != filePathMap.end()) {
        imageInfo.image = cv::imread(filePathMap[imageId]);
        imageInfo.imageId = imageId;
    }
    else {
        imageInfo.imageId = 0;
    }
    return imageInfo;
}

size_t LocalImageLoader::getTotalCount() {
    return totalCnt;
}

size_t LocalImageLoader::getCurrentIndex() {
    return currIdx;
}
