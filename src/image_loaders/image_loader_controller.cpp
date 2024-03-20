#include "image_loaders/image_loader_controller.h"
#include <random>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include "utils/random_utils.h"
#include "config/config.h"
#include "utils/log.h"

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

ImageLoaderController* ImageLoaderController::instance = nullptr;
pthread_mutex_t ImageLoaderController::lock;

void ImageLoaderController::ConnectionInfo::updateTime() {
    lastRequestTime = std::chrono::steady_clock::now();
}

ImageLoaderController::ImageLoaderController() {
    auto config = Config::getSingletonInstance();
    connectionTimeout = config->getImageLoaderTimeout();
}

void ImageLoaderController::checkConnections() {
    while (1) {
        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        std::vector<int64_t> eraseConnectionIds;
        for (auto& entry : connectionsMap) {
            int64_t connectionId = entry.first;
            ConnectionInfo& info = entry.second;
            // 如果正在使用，则跳过
            if (info.userCnt) {
                continue;
            }
            std::chrono::duration<double> elapsedTime = currentTime - info.lastRequestTime;
            if (elapsedTime.count() >= connectionTimeout) {
                eraseConnectionIds.emplace_back(connectionId);
            }
        }
        for (auto& eraseConnectionId : eraseConnectionIds) {
            LOG("connection %lld timeout\n", eraseConnectionId);
            disconnectImageLoader(eraseConnectionId);
        }
        std::this_thread::sleep_for(std::chrono::seconds(connectionTimeout / 2));
    }
}

void ImageLoaderController::startCheckConnections() {
    // checkConnectionsThread = std::thread(&ImageLoaderController::checkConnections, this);
    // checkConnectionsThread.detach();
} 

// 懒汉单例模式
ImageLoaderController* ImageLoaderController::getSingletonInstance() {
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

ImageLoaderBase* ImageLoaderController::getImageLoader(int64_t connectionId) {
    auto loaderArgsHashIt = connectionsMap.find(connectionId);
    if (connectionsMap.end() == loaderArgsHashIt) return nullptr;
    auto imageLoaderIt = loadersMap.find(loaderArgsHashIt->second.loaderArgsHash);
    loaderArgsHashIt->second.updateTime();
    if (loadersMap.end() == imageLoaderIt) return nullptr;
    return imageLoaderIt->second.ptr;
}

bool ImageLoaderController::startUsingLoader(int64_t connectionId) {
    auto connectionIt = connectionsMap.find(connectionId);
    if (connectionsMap.end() == connectionIt) return false;
    pthread_mutex_lock(&(connectionIt->second.userCntLock));
    ++connectionIt->second.userCnt;
    pthread_mutex_unlock(&(connectionIt->second.userCntLock));
    connectionIt->second.updateTime();
    return true;
}

bool ImageLoaderController::stopUsingLoader(int64_t connectionId) {
    auto connectionIt = connectionsMap.find(connectionId);
    if (connectionsMap.end() == connectionIt) return false;
    pthread_mutex_lock(&(connectionIt->second.userCntLock));
    --connectionIt->second.userCnt;
    pthread_mutex_unlock(&(connectionIt->second.userCntLock));
    connectionIt->second.updateTime();
    return true;
}

bool ImageLoaderController::initImageLoader(std::unordered_map<std::string, std::string> args, ImageLoaderFactory::SourceType type, bool isUnique, int64_t &outLoaderArgsHash) {
    LOG("initImageLoader\n");
    
    // TODO: 暂时粗暴地解决线程安全问题
    static pthread_mutex_t connectImageLoaderLock;
    pthread_mutex_lock(&connectImageLoaderLock);
    // 用hash值查找已建立的加载器
    if (0 != outLoaderArgsHash) {
        auto imageLoaderIt = loadersMap.find(outLoaderArgsHash);
        // 没有找到，直接返回错误
        if (loadersMap.end() == imageLoaderIt) {
            pthread_mutex_unlock(&connectImageLoaderLock);
            return false;
        }
        // 重复初始化 跳过
        else {
            pthread_mutex_unlock(&connectImageLoaderLock);
            return true;
        }
    }
    else {
        std::cout << "type: " << static_cast<int>(type) << std::endl;
        std::cout << "args: " << std::endl;
        for (auto arg: args) {
            std::cout << "  " << arg.first << ": " << arg.second << std::endl;
        }
        // TODO: 未考虑哈希冲突
        outLoaderArgsHash = hashArgs(static_cast<int>(type), args);
        auto imageLoaderIt = loadersMap.find(outLoaderArgsHash);
        bool needCreate = false;
        // 没有这个源的信息，新建
        if (loadersMap.end() == imageLoaderIt) {
            std::cout << "not found, creat" << std::endl;
            needCreate = true;
        }
        // 有这个源的信息，且不是独占
        else if (!isUnique) {
            pthread_mutex_unlock(&connectImageLoaderLock);
            return true;
        }
        // 有这个源的信息，且是独占，新建
        else {
            std::cout << "unique, creat" << std::endl;
            // 判断这个ID是否重复
            do {
                auto current = std::chrono::system_clock::now().time_since_epoch();
                auto seed = static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(current).count());
                outLoaderArgsHash = hashArgs(static_cast<int>(type), args, seed);
            } while (loadersMap.find(outLoaderArgsHash) != loadersMap.end());
            needCreate = true;
        }
        if (needCreate) {
            loadersMap.emplace(outLoaderArgsHash, ImageLoaderController::ImageLoaderInfo());
            loadersMap[outLoaderArgsHash].ptr = ImageLoaderFactory::createImageLoader(type);
            for (auto arg: args) {
                loadersMap[outLoaderArgsHash].ptr->setArgument(arg.first, arg.second);
            }
            if (!loadersMap[outLoaderArgsHash].ptr->start()) {
                delete loadersMap[outLoaderArgsHash].ptr;
                loadersMap.erase(outLoaderArgsHash);
                std::cout << "image loader start failed" << std::endl;
                return false;
            }
        }
    }
    std::cout << std::endl
              << "init image loader" << std::endl
              << "loader args hash:" << outLoaderArgsHash << std::endl
              << "connect cnt:" << loadersMap[outLoaderArgsHash].cnt << std::endl;
    pthread_mutex_unlock(&connectImageLoaderLock);
    return true;
}

bool ImageLoaderController::connectImageLoader(int64_t loaderArgsHash, int64_t &outConnectionId) {
    LOG("connectImageLoader\n");
    
    // TODO: 暂时粗暴地解决线程安全问题
    static pthread_mutex_t connectImageLoaderLock;
    pthread_mutex_lock(&connectImageLoaderLock);
    // 用hash值查找已建立的加载器
    if (0 == loaderArgsHash) {
        return false;
    }
    auto imageLoaderIt = loadersMap.find(loaderArgsHash);
    // 没有找到，直接返回错误
    if (loadersMap.end() == imageLoaderIt) {
        return false;
    }
    ++loadersMap[loaderArgsHash].cnt;

    outConnectionId = generateInt64Random();
    std::cout << std::endl
              << "connect image loader" << std::endl
              << "connection ID:" << outConnectionId << std::endl
              << "loader args hash:" << loaderArgsHash << std::endl
              << "connect cnt:" << loadersMap[loaderArgsHash].cnt << std::endl;
    // TODO: 未考虑哈希冲突，以后在分布式ID生成器中统一解决
    connectionsMap.emplace(outConnectionId, ImageLoaderController::ConnectionInfo());
    connectionsMap[outConnectionId].loaderArgsHash = loaderArgsHash;
    connectionsMap[outConnectionId].updateTime();
    pthread_mutex_unlock(&connectImageLoaderLock);
    return true;
}

bool ImageLoaderController::disconnectImageLoader(int64_t connectionId) {
    LOG("disconnectImageLoader\n");
    auto connectionInfoIt = connectionsMap.find(connectionId);
    if (connectionsMap.end() == connectionInfoIt) return false;
    connectionsMap.erase(connectionInfoIt);

    auto imageLoaderIt = loadersMap.find(connectionInfoIt->second.loaderArgsHash);
    if (loadersMap.end() == imageLoaderIt) return false;

    int64_t loaderArgsHash = connectionInfoIt->second.loaderArgsHash;
    --loadersMap[loaderArgsHash].cnt;
    std::cout << std::endl
              << "disconnect image loader" << std::endl
              << "connection ID:" << connectionId << std::endl
              << "loader args hash:" << loaderArgsHash << std::endl
              << "connect cnt:" << loadersMap[loaderArgsHash].cnt << std::endl;
    if (loadersMap[loaderArgsHash].cnt <= 0) {
        delete loadersMap[loaderArgsHash].ptr;
        loadersMap.erase(imageLoaderIt);
    }
    return true;
}
void ImageLoaderController::setConnectionTimeout(int timeout) {
    connectionTimeout = timeout;
}
