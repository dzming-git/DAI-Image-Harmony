#include "image_loaders/image_loader_controller.h"
#include <random>
#include <iostream>
#include <chrono>

#define LOG(fmt, ...) printf("[%s : %d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

std::int64_t hashArgs(int type, const std::vector<std::pair<std::string, std::string>>& args, std::size_t seed = 0) {
    auto hashString = [&seed](const std::string& str) {
        seed ^= std::hash<std::string>()(str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };
    hashString(std::to_string(type));
    for (const auto& arg : args) {
        hashString(arg.first + "\n" + arg.second);
    }
    return static_cast<std::size_t>(seed);
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

int64_t ImageLoaderController::registerImageLoader(std::vector<std::pair<std::string, std::string>> args, ImageLoaderFactory::SourceType type) {
    LOG("registerImageLoader\n");
    std::cout << "type: " << static_cast<int>(type) << std::endl;
    std::cout << "args: " << std::endl;
    for (auto arg: args) {
        std::cout << "  " << arg.first << ": " << arg.second << std::endl;
    }
    // TODO: 暂时粗暴地解决线程安全问题
    static pthread_mutex_t registerImageLoaderLock;
    pthread_mutex_lock(&registerImageLoaderLock);
    // TODO: 未考虑哈希冲突
    int64_t connectId = hashArgs(static_cast<int>(type), args);
    auto imageLoaderIt = loadersMap.find(connectId);
    bool needCreate = false;
    // 没有这个源的信息，新建
    if (loadersMap.end() == imageLoaderIt) {
        std::cout << "not found, creat" << std::endl;
        needCreate = true;
    }
    // 有这个源的信息，且不是独占，数量+1
    else if (false == loadersMap[connectId].ptr->isUnique()) {
        ++loadersMap[connectId].cnt;
    }
    // 有这个源的信息，且是独占，新建
    else {
        std::cout << "unique, creat" << std::endl;
        // 判断这个ID是否重复
        do {
            auto current = std::chrono::system_clock::now().time_since_epoch();
            auto seed = static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(current).count());
            connectId = hashArgs(static_cast<int>(type), args, seed);
        } while (loadersMap.find(connectId) != loadersMap.end());
        needCreate = true;
    }
    if (needCreate) {
        loadersMap.emplace(connectId, ImageLoaderController::ImageLoaderPtrAndCount());
        loadersMap[connectId].ptr = ImageLoaderFactory::createImageLoader(type);
        for (auto arg: args) {
            loadersMap[connectId].ptr->setArgument(arg.first, arg.second);
        }
        if (loadersMap[connectId].ptr->start()) {
            loadersMap[connectId].cnt = 1;
        }
        else {
            unregisterImageLoader(connectId);
            std::cout << "image loader start failed" << std::endl;
            return -1;
        }
    }
    std::cout << "connect cnt:" << loadersMap[connectId].cnt << std::endl;
    pthread_mutex_unlock(&registerImageLoaderLock);
    return connectId;
}

void ImageLoaderController::unregisterImageLoader(int64_t connectId) {
    LOG("unregisterImageLoader\n");
    auto imageLoaderIt = loadersMap.find(connectId);
    if (loadersMap.end() != imageLoaderIt) {
        --loadersMap[connectId].cnt;
        std::cout << "connect cnt:" << loadersMap[connectId].cnt << std::endl;
        if (loadersMap[connectId].cnt <= 0) {
            delete loadersMap[connectId].ptr;
            loadersMap.erase(imageLoaderIt);
            
        }
    } 
}
