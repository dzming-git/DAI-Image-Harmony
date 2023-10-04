#include "image_loaders/image_loader_controller.h"
#include <random>

std::size_t hashCombine(std::size_t seed, const std::string& str) {
    std::hash<std::string> hasher;
    return seed ^= hasher(str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

std::int64_t hashVector(const std::vector<std::string>& vec) {
    std::int64_t hash_value = 0;
    for (const auto& str : vec) {
        hash_value = hashCombine(hash_value, str);
    }
    return hash_value;
}

int64_t generateRandomInt64() {
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<int64_t> distribution(
        std::numeric_limits<int64_t>::min(),
        std::numeric_limits<int64_t>::max()
    );
    return distribution(generator);
}

ImageLoaderController* ImageLoaderController::instance = nullptr;
pthread_mutex_t ImageLoaderController::lock;

ImageLoaderController::ImageLoaderController() {
    
}

// 懒汉单例模式
ImageLoaderController *ImageLoaderController::getSingletonInstance() {
	if (nullptr == instance) {
		pthread_mutex_lock(&lock);
		if (nullptr == instance) {
			instance = new ImageLoaderController();
		}
        pthread_mutex_unlock(&lock);
	}
	return instance;
}

ImageLoaderBase *ImageLoaderController::getImageLoader(int64_t connectId) {
    auto imageLoaderIt = loadersMap.find(connectId);
    if (loadersMap.end() == imageLoaderIt) {
        return nullptr;
    }
    return imageLoaderIt->second.ptr;
}

int64_t ImageLoaderController::registerImageLoader(std::vector<std::string> sources, ImageLoaderFactory::SourceType type) {
    // TODO: 未考虑哈希冲突
    int64_t connectId = hashVector(sources);
    auto imageLoaderIt = loadersMap.find(connectId);
    bool needCreate = false;
    // 没有这个源的信息，新建
    if (loadersMap.end() == imageLoaderIt) {
        needCreate = true;
    }
    // 有这个源的信息，且不是独占，数量+1
    else if (false == loadersMap[connectId].ptr->isUnique()) {
        ++loadersMap[connectId].cnt;
    }
    // 有这个源的信息，且是独占，新建
    else {
        // 判断这个ID是否重复
        do {
            connectId = generateRandomInt64();
        } while (loadersMap.find(connectId) != loadersMap.end());
        needCreate = true;
    }
    if (needCreate) {
        loadersMap.emplace(connectId, ImageLoaderController::ImageLoaderPtrAndCount());
        loadersMap[connectId].ptr = ImageLoaderFactory::createImageLoader(type);
        loadersMap[connectId].ptr->setSource(sources);
        loadersMap[connectId].cnt = 1;
    }
    return connectId;
}

void ImageLoaderController::unregisterImageLoader(int64_t connectId) {
    auto imageLoaderIt = loadersMap.find(connectId);
    if (loadersMap.end() != imageLoaderIt) { 
        if (--loadersMap[connectId].cnt <= 0) {
            delete loadersMap[connectId].ptr;
            loadersMap.erase(imageLoaderIt);
        }
    }
}
