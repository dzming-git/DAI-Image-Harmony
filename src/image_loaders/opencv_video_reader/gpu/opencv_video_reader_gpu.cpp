#include "image_loaders/opencv_video_reader/gpu/opencv_video_reader_gpu.h"
#include <iostream>
#include <sstream>
#include <random>
#include <thread>
#include "config/config.h"
#include "utils/random_utils.h"

#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudacodec.hpp>
#include "utils/log.h"

class OpencvVideoReader_GPU::VideoBufInfo {
public:
    VideoBufInfo();
    ~VideoBufInfo();
    std::string url;
    cv::Ptr<cv::cudacodec::VideoReader> cap;
    int h;
    int w;
    int bufLen;
    double fps;
    char* bufShallowcopy;
    int historyMaxSize;  // 内存池最多缓存多少图片
    char* historyFrameMemoryPool;  // 内存池
    int historyFrameMemoryPoolUpdateIndex;  // 当前需要写入的位置
    std::unordered_map<int64_t, char*> history;  // 通过时间戳查询图片指针的哈希表
    std::queue<int64_t> historyOrder;  // 按照顺序记录时间戳，目的是缓存满时删除最早图片
    pthread_mutex_t historyLock;
};

OpencvVideoReader_GPU::OpencvVideoReader_GPU(): totalCnt(0), currIdx(0) {
    std::cout << "Create OpencvVideoReader_GPU" << std::endl;
    args.emplace("URL", "");
    videoBufInfo = new OpencvVideoReader_GPU::VideoBufInfo;
}

OpencvVideoReader_GPU::~OpencvVideoReader_GPU() {
    std::cout << "Destroy OpencvVideoReader_GPU" << std::endl;
    videoReadThreadStop = true;
    if (videoBufInfo) {
        delete videoBufInfo;
    }
}

bool OpencvVideoReader_GPU::setArgument(std::string key, std::string value) {
    if (args.end() == args.find(key)) return false;
    args[key] = value;
    return true;
}

void OpencvVideoReader_GPU::videoReadThreadFunc(OpencvVideoReader_GPU::VideoBufInfo* videoBufInfo, bool* videoReadThreadStop) {
    int frameIndex = 0;
    int frameSize = videoBufInfo->w * videoBufInfo->h * 3;
    int loopTimeMs = 1000 / videoBufInfo->fps;
    while (!(*videoReadThreadStop)) {
        try {
            auto start = std::chrono::steady_clock::now();
            int64_t imageId = generateInt64Random();
            cv::cuda::GpuMat frameGPU;
            if (videoBufInfo->cap->nextFrame(frameGPU)) {
                if (nullptr == videoBufInfo->historyFrameMemoryPool) {
                    auto frameSize = frameGPU.size();
                    videoBufInfo->w = frameSize.width;
                    videoBufInfo->h = frameSize.height;
                    videoBufInfo->bufLen = 3 * videoBufInfo->w * videoBufInfo->h;
                    videoBufInfo->historyFrameMemoryPool = new char[videoBufInfo->bufLen * videoBufInfo->historyMaxSize];
                }
                // 方法1 会导致HCNetSDK init时报错，错误码3
                // cv::cuda::cvtColor(frameGPU, frameGPU, cv::COLOR_RGBA2RGB);
                // 方法2 会导致HCNetSDK启动解码回调后卡死
                // cv::cuda::GpuMat RGBA[4];
                // cv::cuda::split(frameGPU, RGBA);
                // cv::cuda::merge(RGBA, 3, frameGPU);
                // 方法3 先下载到内存，再用CPU cvtColor
                cv::Mat frameTmp;
                frameGPU.download(frameTmp);
                char* currentFrame = videoBufInfo->historyFrameMemoryPool + (frameIndex * frameSize);
                cv::Mat frame(videoBufInfo->h, videoBufInfo->w, CV_8UC3, currentFrame);
                cv::cvtColor(frameTmp, frame, cv::COLOR_RGBA2RGB);
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
            auto end = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            int executionTimeMs = elapsed.count();
            int delayTimeMs = loopTimeMs - executionTimeMs;
            if (delayTimeMs > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(delayTimeMs));
            }
        } catch (const std::exception& e) {
            videoBufInfo->cap = cv::cudacodec::createVideoReader(videoBufInfo->url);
            std::cout << WHERE << e.what() << std::endl;
        }
    }
}

bool OpencvVideoReader_GPU::start() {
    videoBufInfo->url = args["URL"];
    // TODO 不知道cuda的VideoReader怎么获取这一参数
    cv::VideoCapture capTmp(videoBufInfo->url);
    if (!capTmp.isOpened()) {
        return false;
    }
    videoBufInfo->fps = capTmp.get(cv::CAP_PROP_FPS);
    capTmp.release();

    videoBufInfo->cap = cv::cudacodec::createVideoReader(videoBufInfo->url);
    if (!videoBufInfo->cap) {
        return false;
    }
    videoReadThreadStop = false;
    std::thread videoReadThread(videoReadThreadFunc, videoBufInfo, &videoReadThreadStop);
    videoReadThread.detach();
    return true;
}

bool OpencvVideoReader_GPU::hasNext() {
    return static_cast<bool>(videoBufInfo->cap);
}

ImageInfo OpencvVideoReader_GPU::next(int64_t previousImageId) {
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

ImageInfo OpencvVideoReader_GPU::getImageById(int64_t imageId) {
    ImageInfo imageInfo;
    if (!videoBufInfo->historyOrder.empty() && 0 == imageId) {
        imageId = videoBufInfo->historyOrder.back();
    }
    char* historyFrameMemoryPoolOffset = videoBufInfo->history[imageId];
    imageInfo.image = cv::Mat(videoBufInfo->h, videoBufInfo->w, CV_8UC3, historyFrameMemoryPoolOffset);
    imageInfo.imageId = imageId;
    return imageInfo;
}

size_t OpencvVideoReader_GPU::getTotalCount() {
    return totalCnt;
}

size_t OpencvVideoReader_GPU::getCurrentIndex() {
    return currIdx;
}

OpencvVideoReader_GPU::VideoBufInfo::VideoBufInfo():
h(0), w(0), bufLen(0), bufShallowcopy(nullptr), historyFrameMemoryPool(nullptr), historyFrameMemoryPoolUpdateIndex(0) {
    auto config = Config::getSingletonInstance();
    historyMaxSize = config->getHistoryMaxSize();
}

OpencvVideoReader_GPU::VideoBufInfo::~VideoBufInfo() {
    if (historyFrameMemoryPool) {
        delete historyFrameMemoryPool;
    }
}
