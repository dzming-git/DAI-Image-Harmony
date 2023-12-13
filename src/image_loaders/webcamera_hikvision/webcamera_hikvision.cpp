#include "image_loaders/webcamera_hikvision/webcamera_hikvision.h"
#include "HCNetSDK.h"
#include "LinuxPlayM4.h"
#include <unistd.h>
#include <chrono>
#include <iostream>
#include "config/config.h"

#define LOG(fmt, ...) printf("[%s : %d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

void CALLBACK DecCBFun(int, char* pBuf, int, FRAME_INFO* pFrameInfo, void* videoBufInfo, int) {
    // TODO: 临时用本地生成的时间戳，未来再接入时间同步器
    auto now = std::chrono::system_clock::now();
    auto timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    WebcameraHikvisionLoader::VideoBufInfo* info = (WebcameraHikvisionLoader::VideoBufInfo*)videoBufInfo;
    if (T_YV12 == pFrameInfo->nType) {
        if (nullptr == ((WebcameraHikvisionLoader::VideoBufInfo*)videoBufInfo)->bufShallowcopy) {
            info->h = pFrameInfo->nHeight;
            info->w = pFrameInfo->nWidth;
            info->bufLen = (pFrameInfo->nHeight + pFrameInfo->nHeight / 2) * pFrameInfo->nWidth;
            info->bufShallowcopy = pBuf;
            info->historyFrameMemoryPool = new char[info->bufLen * info->historyMaxSize];
        }
        // info->updated = true;
        // TODO  未考虑使用过程中清晰度改变
        int soloFrameMemoryLen = 3 * info->h * info->w;

        // 循环存储
        ++info->historyFrameMemoryPoolUpdateIndex;
        info->historyFrameMemoryPoolUpdateIndex %= info->historyMaxSize;
        
        char* historyFrameMemoryPoolOffset = info->historyFrameMemoryPool + info->historyFrameMemoryPoolUpdateIndex * info->bufLen;
        memcpy(historyFrameMemoryPoolOffset, pBuf, info->bufLen);
        pthread_mutex_lock(&info->historyLock);
        if (info->historyOrder.size() >= info->historyMaxSize) {
            info->history.erase(info->historyOrder.front());
            info->historyOrder.pop();
        }
        info->historyOrder.emplace(timeStamp);
        info->history[timeStamp] = historyFrameMemoryPoolOffset;
        pthread_mutex_unlock(&info->historyLock);
    }
}

void CALLBACK fRealDataCallBack_V30(LONG, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, void* p_nPort) {
    int nPort = *(int*)p_nPort;
    if (false == (\
        NET_DVR_STREAMDATA == dwDataType && \
        dwBufSize > 0 && nPort != -1 \
        )) {
        return;
    }
    PlayM4_InputData(nPort, pBuffer, dwBufSize);
}

WebcameraHikvisionLoader::WebcameraHikvisionLoader(): 
totalCnt(0), currIdx(0), userId(-1), playOk(false) {
    std::cout << "Create WebcameraHikvisionLoader" << std::endl;
    initOk = NET_DVR_Init();
    initOk &= NET_DVR_SetConnectTime(2000, 1);
    initOk &= NET_DVR_SetReconnect(10000, true);
    videoBufInfo = new WebcameraHikvisionLoader::VideoBufInfo;
    videoBufInfo->bufShallowcopy = nullptr;
    

    args.emplace("DeviceAddress", "");
    args.emplace("UserName", "admin");
    args.emplace("Password", "");
    args.emplace("Port", "8000");
}

WebcameraHikvisionLoader::~WebcameraHikvisionLoader() {
    std::cout << "Destroy WebcameraHikvisionLoader" << std::endl;
    if (videoBufInfo) {
        NET_DVR_StopRealPlay(handle);			//关闭预览
        NET_DVR_Logout(userId);					//注销用户
        NET_DVR_Cleanup();						//释放SDK资源
        delete videoBufInfo;
    }
}

bool WebcameraHikvisionLoader::setArgument(std::string key, std::string value) {
    if (args.end() == args.find(key)) return false;
    args[key] = value;
    return true;
}

bool WebcameraHikvisionLoader::start() {
    NET_DVR_USER_LOGIN_INFO pLoginInfo = {0};
    NET_DVR_DEVICEINFO_V40 lpDeviceInfo = {0};

    pLoginInfo.bUseAsynLogin = 0;  // 同步登录方式
    strcpy(pLoginInfo.sDeviceAddress, args["DeviceAddress"].c_str());
    strcpy(pLoginInfo.sUserName, args["UserName"].c_str());
    strcpy(pLoginInfo.sPassword, args["Password"].c_str());
    pLoginInfo.wPort = atoi(args["Port"].c_str());

    userId = NET_DVR_Login_V40(&pLoginInfo, &lpDeviceInfo);
    if (userId < 0) {
        LOG("登录摄像头出错, 错误代码：%d\n", (int)NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        return false;
    }

    //设置解码回调
    bool flag = true;
    flag &= PlayM4_GetPort(&nPort);  // 获取播放库通道号
    flag &= PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME);  // 设置流模式
    flag &= PlayM4_OpenStream(nPort, nullptr, 0, 1024 * 1024);  // 打开流
    flag &= PlayM4_SetDecCallBackMend(nPort, DecCBFun, videoBufInfo);  // 设置视频解码回调函数
    flag &= PlayM4_Play(nPort, 0);  // 开始播放
    if (false == flag) {
        LOG();
        NET_DVR_Cleanup();
        PlayM4_FreePort(nPort);
        return false;  // 设置解码回调失败，返回
    }
    //启动实时预览，设置实施回调
    NET_DVR_PREVIEWINFO struPlayInfo = {0};
    struPlayInfo.hPlayWnd = 0; //窗口为空，设备SDK不解码只取流
    struPlayInfo.lChannel = 1; //Channel number 设备通道
    // 码流类型，0-主码流，1-子码流，2-码流3，3-码流4, 4-码流5,5-码流6,7-码流7,8-码流8,9-码流9,10-码流10
    struPlayInfo.dwStreamType = 0;
    // 0-TCP方式,1-UDP方式,2-多播方式,3-RTP方式，4-RTP/RTSP,5-RSTP/HTTP
    struPlayInfo.dwLinkMode = 0;
    // 0-非阻塞取流, 1-阻塞取流
    struPlayInfo.bBlocked = 0;
    handle = NET_DVR_RealPlay_V40(userId, &struPlayInfo, fRealDataCallBack_V30, &nPort);
    if (handle < 0) {
        LOG("实时预览调用出错，错误代码：%d\n", (int)NET_DVR_GetLastError());
        NET_DVR_Logout(userId);
        NET_DVR_Cleanup();
        return false;  // 实时播放失败，返回
    }
    auto start = std::chrono::steady_clock::now();
    while (nullptr == videoBufInfo->bufShallowcopy)  // 关键一步，等待回调函数DecCBFun运行之后才可以进行之后操作
    {
        auto end = std::chrono::steady_clock::now();  // 记录结束时间
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (duration > 2000) { // 2秒未成功，超时
            LOG("回调函数启动失败，错误代码：%d\n", (int)NET_DVR_GetLastError());
            NET_DVR_StopRealPlay(handle);
            NET_DVR_Logout(userId);
            NET_DVR_Cleanup();
            std::cout << PlayM4_GetLastError(nPort) << std::endl;
            return false;  // 回调函数启动失败，返回
        }
        sleep(1);
    }
    playOk = true;
    return true;
}

bool WebcameraHikvisionLoader::isUnique() {
    return false;
}

bool WebcameraHikvisionLoader::hasNext() {
    return playOk;
    // return playOk && videoBufInfo->updated;
}

ImageInfo WebcameraHikvisionLoader::next(int64_t previousImageId) {
    // videoBufInfo->updated = false;
    ImageInfo imageInfo;
    if (nullptr == videoBufInfo->bufShallowcopy) {
        return imageInfo;
    }
    if (videoBufInfo->historyOrder.empty()) {
        return imageInfo;
    }
    imageInfo.imageId = videoBufInfo->historyOrder.back();
    char* historyFrameMemoryPoolOffset = videoBufInfo->history[imageInfo.imageId];
    if (historyFrameMemoryPoolOffset) {
        cv::Mat imageYUV420(videoBufInfo->h + videoBufInfo->h / 2, videoBufInfo->w, CV_8UC1, historyFrameMemoryPoolOffset);
        cv::cvtColor(imageYUV420, imageInfo.image, cv::COLOR_YUV2BGR_YV12);
    }
    else {
        imageInfo.imageId = 0;
    }
    return imageInfo;
}

ImageInfo WebcameraHikvisionLoader::getImageById(int64_t imageId) {
    ImageInfo imageInfo;
    char* historyFrameMemoryPoolOffset = videoBufInfo->history[imageId];
    cv::Mat imageYUV420(videoBufInfo->h + videoBufInfo->h / 2, videoBufInfo->w, CV_8UC1, historyFrameMemoryPoolOffset);
    cv::cvtColor(imageYUV420, imageInfo.image, cv::COLOR_YUV2BGR_YV12);
    imageInfo.imageId = imageId;
    return imageInfo;
}

size_t WebcameraHikvisionLoader::getTotalCount() {
    return totalCnt;
}

size_t WebcameraHikvisionLoader::getCurrentIndex() {
    return currIdx;
}

WebcameraHikvisionLoader::VideoBufInfo::VideoBufInfo():
h(0), w(0), bufLen(0), updated(false), bufShallowcopy(nullptr), historyFrameMemoryPool(nullptr), historyFrameMemoryPoolUpdateIndex(0) {
    auto config = Config::getSingletonInstance();
    historyMaxSize = config->getHistoryMaxSize();
}

WebcameraHikvisionLoader::VideoBufInfo::~VideoBufInfo() {
    if (historyFrameMemoryPool) {
        delete historyFrameMemoryPool;
    }
}
