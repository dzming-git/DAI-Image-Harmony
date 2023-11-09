#include "image_loaders/image_loader_controller.h"
#include <random>
#include <iostream>
#include <chrono>
#include <unordered_map>

#define LOG(fmt, ...) printf("[%s : %d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

std::int64_t hashArgs(int type, const std::unordered_map<std::string, std::string>& args, std::size_t seed = 0) {
    auto hashString = [&seed](const std::string& str) {
        seed ^= std::hash<std::string>()(str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };
    hashString(std::to_string(type));
    for (const auto& arg : args) {
        hashString(arg.first + "\n" + arg.second);
    }
    return static_cast<std::size_t>(seed);
}

// TODO: 未来使用分布式ID生成器实现
std::int64_t generateInt64Random() {
    auto time = std::chrono::system_clock::now();
    auto duration = time.time_since_epoch();
    auto seed = static_cast<unsigned int>(duration.count());
    std::mt19937_64 rng(seed);
    
    // 生成int64_t范围内的随机数
    std::uniform_int_distribution<int64_t> dist(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());
    return dist(rng);
}

ImageLoaderController* ImageLoaderController::instance = nullptr;
pthread_mutex_t ImageLoaderController::lock;

void ImageLoaderController::ConnectionInfo::updateTime() {
    lastRequestTime = std::chrono::steady_clock::now();
}

ImageLoaderController::ImageLoaderController(): connectionTimeout(10) {
}

void ImageLoaderController::checkConnections() {
    while (1) {
        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        std::vector<int64_t> eraseConnectionIds;
        for (auto& entry : connectionsMap) {
            int64_t connectionId = entry.first;
            ConnectionInfo& info = entry.second;

            std::chrono::duration<double> elapsedTime = currentTime - info.lastRequestTime;
            if (elapsedTime.count() >= connectionTimeout) {
                eraseConnectionIds.emplace_back(connectionId);
            }
        }
        for (auto& eraseConnectionId : eraseConnectionIds) {
            LOG("connection %lld timeout\n", eraseConnectionId);
            unregisterImageLoader(eraseConnectionId);
        }
        std::this_thread::sleep_for(std::chrono::seconds(connectionTimeout / 2));
    }
}

void ImageLoaderController::startCheckConnections() {
    checkConnectionsThread = std::thread(&ImageLoaderController::checkConnections, this);
    checkConnectionsThread.detach();
} 

// 懒汉单例模式
ImageLoaderController *ImageLoaderController::getSingletonInstance() {
	if (nullptr == instance) {
		pthread_mutex_lock(&lock);
		if (nullptr == instance) {
			instance = new ImageLoaderController();
            instance->startCheckConnections();
		}
        pthread_mutex_unlock(&lock);
	}
	return instance;
}

ImageLoaderBase *ImageLoaderController::getImageLoader(int64_t connectionId) {
    auto loaderArgsHashIt = connectionsMap.find(connectionId);
    if (connectionsMap.end() == loaderArgsHashIt) return nullptr;
    auto imageLoaderIt = loadersMap.find(loaderArgsHashIt->second.loaderArgsHash);
    loaderArgsHashIt->second.updateTime();
    if (loadersMap.end() == imageLoaderIt) return nullptr;
    return imageLoaderIt->second.ptr;
}

int64_t ImageLoaderController::registerImageLoader(std::unordered_map<std::string, std::string> args, ImageLoaderFactory::SourceType type) {
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
    int64_t loaderArgsHash = hashArgs(static_cast<int>(type), args);
    auto imageLoaderIt = loadersMap.find(loaderArgsHash);
    bool needCreate = false;
    // 没有这个源的信息，新建
    if (loadersMap.end() == imageLoaderIt) {
        std::cout << "not found, creat" << std::endl;
        needCreate = true;
    }
    // 有这个源的信息，且不是独占，数量+1
    else if (false == loadersMap[loaderArgsHash].ptr->isUnique()) {
        ++loadersMap[loaderArgsHash].cnt;
    }
    // 有这个源的信息，且是独占，新建
    else {
        std::cout << "unique, creat" << std::endl;
        // 判断这个ID是否重复
        do {
            auto current = std::chrono::system_clock::now().time_since_epoch();
            auto seed = static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(current).count());
            loaderArgsHash = hashArgs(static_cast<int>(type), args, seed);
        } while (loadersMap.find(loaderArgsHash) != loadersMap.end());
        needCreate = true;
    }
    if (needCreate) {
        loadersMap.emplace(loaderArgsHash, ImageLoaderController::ImageLoaderInfo());
        loadersMap[loaderArgsHash].ptr = ImageLoaderFactory::createImageLoader(type);
        for (auto arg: args) {
            loadersMap[loaderArgsHash].ptr->setArgument(arg.first, arg.second);
        }
        if (loadersMap[loaderArgsHash].ptr->start()) {
            loadersMap[loaderArgsHash].cnt = 1;
        }
        else {
            unregisterImageLoader(loaderArgsHash);
            std::cout << "image loader start failed" << std::endl;
            return -1;
        }
    }
    std::cout << "connect cnt:" << loadersMap[loaderArgsHash].cnt << std::endl;
    pthread_mutex_unlock(&registerImageLoaderLock);
    int64_t connectionId = generateInt64Random();
    // TODO: 未考虑哈希冲突，以后在分布式ID生成器中统一解决
    connectionsMap.emplace(connectionId, ImageLoaderController::ConnectionInfo());
    connectionsMap[connectionId].loaderArgsHash = loaderArgsHash;
    connectionsMap[connectionId].updateTime();
    return connectionId;
}

bool ImageLoaderController::unregisterImageLoader(int64_t connectionId) {
    LOG("unregisterImageLoader\n");
    auto loaderArgsHashIt = connectionsMap.find(connectionId);
    if (connectionsMap.end() == loaderArgsHashIt) return false;

    auto imageLoaderIt = loadersMap.find(loaderArgsHashIt->second.loaderArgsHash);
    loaderArgsHashIt->second.updateTime();
    if (loadersMap.end() == imageLoaderIt) return false;

    int64_t loaderArgsHash = loaderArgsHashIt->second.loaderArgsHash;
    --loadersMap[loaderArgsHash].cnt;
    std::cout << "connect cnt:" << loadersMap[loaderArgsHash].cnt << std::endl;
    if (loadersMap[loaderArgsHash].cnt <= 0) {
        delete loadersMap[loaderArgsHash].ptr;
        loadersMap.erase(imageLoaderIt);
    }
    connectionsMap.erase(loaderArgsHashIt);
    return true;
}
void ImageLoaderController::setConnectionTimeout(int timeout) {
    connectionTimeout = timeout;
}
