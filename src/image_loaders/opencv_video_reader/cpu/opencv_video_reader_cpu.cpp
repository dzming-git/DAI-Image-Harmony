#include "image_loaders/opencv_video_reader/cpu/opencv_video_reader_cpu.h"
#include <iostream>
#include <sstream>
#include <random>
#include <thread>
#include "config/config.h"
#include "utils/random_utils.h"

class OpencvVideoReader_CPU::VideoBufInfo {
public:
    VideoBufInfo();
    ~VideoBufInfo();
    cv::VideoCapture cap;

    int h;
    int w;
    int bufLen;
    bool updated;
    char* bufShallowcopy;
    int historyMaxSize;  // 内存池最多缓存多少图片
    char* historyFrameMemoryPool;  // 内存池
    int historyFrameMemoryPoolUpdateIndex;  // 当前需要写入的位置
    std::unordered_map<int64_t, char*> history;  // 通过时间戳查询图片指针的哈希表
    std::queue<int64_t> historyOrder;  // 按照顺序记录时间戳，目的是缓存满时删除最早图片
    pthread_mutex_t historyLock;
};

OpencvVideoReader_CPU::OpencvVideoReader_CPU(): totalCnt(0), currIdx(0) {
    std::cout << "Create OpencvVideoReader_CPU" << std::endl;
    args.emplace("URL", "");
    videoBufInfo = new OpencvVideoReader_CPU::VideoBufInfo;
}

OpencvVideoReader_CPU::~OpencvVideoReader_CPU() {
    std::cout << "Destroy OpencvVideoReader_CPU" << std::endl;
    videoReadThreadStop = true;
    if (videoBufInfo) {
        delete videoBufInfo;
    }
}

bool OpencvVideoReader_CPU::setArgument(std::string key, std::string value) {
    if (args.end() == args.find(key)) return false;
    args[key] = value;
    return true;
}

void OpencvVideoReader_CPU::videoReadThreadFunc(OpencvVideoReader_CPU::VideoBufInfo* videoBufInfo, bool* videoReadThreadStop) {
    int frameIndex = 0;
    int frameSize = videoBufInfo->w * videoBufInfo->h * 3;
    while (!(*videoReadThreadStop)) {
        int64_t imageId = generateInt64Random();
        char* currentFrame = videoBufInfo->historyFrameMemoryPool + (frameIndex * frameSize);
        cv::Mat frame(videoBufInfo->h, videoBufInfo->w, CV_8UC3, currentFrame);
        if (videoBufInfo->cap.read(frame)) {
            frameIndex = (frameIndex + 1) % videoBufInfo->historyMaxSize;
            videoBufInfo->history.emplace(imageId, currentFrame);
            // 检查容器的大小是否超过上限
            if (videoBufInfo->historyOrder.size() >= videoBufInfo->historyMaxSize) {
                // 移除最旧的元素，保持容器大小不超过上限
                videoBufInfo->historyOrder.pop();
            }
            videoBufInfo->historyOrder.push(imageId);
        }
        else {
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 25));
    }
}

bool OpencvVideoReader_CPU::start() {
    videoBufInfo->cap.open(args["URL"]);
    if (videoBufInfo->cap.isOpened()) {
        videoBufInfo->w = videoBufInfo->cap.get(cv::CAP_PROP_FRAME_WIDTH);
        videoBufInfo->h = videoBufInfo->cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        videoBufInfo->bufLen = 3 * videoBufInfo->w * videoBufInfo->h;
        videoBufInfo->historyFrameMemoryPool = new char[videoBufInfo->bufLen * videoBufInfo->historyMaxSize];
        videoReadThreadStop = false;
        std::thread videoReadThread(videoReadThreadFunc, videoBufInfo, &videoReadThreadStop);
        videoReadThread.detach();
        return true;
    }
    return false;
}

bool OpencvVideoReader_CPU::hasNext() {
    return videoBufInfo->cap.isOpened();
}

ImageInfo OpencvVideoReader_CPU::next(int64_t previousImageId) {
    ImageInfo imageInfo;
    if (videoBufInfo->historyOrder.empty()) {
        return imageInfo;
    }
    imageInfo.imageId = videoBufInfo->historyOrder.back();
    char* historyFrameMemoryPoolOffset = videoBufInfo->history[imageInfo.imageId];
    if (historyFrameMemoryPoolOffset) {
        imageInfo.image = cv::Mat(videoBufInfo->h, videoBufInfo->w, CV_8UC3, historyFrameMemoryPoolOffset);
    }
    else {
        imageInfo.imageId = 0;
    }
    return imageInfo;
}

ImageInfo OpencvVideoReader_CPU::getImageById(int64_t imageId) {
    ImageInfo imageInfo;
    char* historyFrameMemoryPoolOffset = videoBufInfo->history[imageId];
    imageInfo.image = cv::Mat(videoBufInfo->h, videoBufInfo->w, CV_8UC3, historyFrameMemoryPoolOffset);
    imageInfo.imageId = imageId;
    return imageInfo;
}

size_t OpencvVideoReader_CPU::getTotalCount() {
    return totalCnt;
}

size_t OpencvVideoReader_CPU::getCurrentIndex() {
    return currIdx;
}

OpencvVideoReader_CPU::VideoBufInfo::VideoBufInfo():
h(0), w(0), bufLen(0), updated(false), bufShallowcopy(nullptr), historyFrameMemoryPool(nullptr), historyFrameMemoryPoolUpdateIndex(0) {
    auto config = Config::getSingletonInstance();
    historyMaxSize = config->getHistoryMaxSize();
}

OpencvVideoReader_CPU::VideoBufInfo::~VideoBufInfo() {
    if (historyFrameMemoryPool) {
        delete historyFrameMemoryPool;
    }
}
