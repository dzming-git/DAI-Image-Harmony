# Image Harmony

D AI Center/集成层/Image Harmony

使用适配器模式+迭代器模式，处理多源数据，输出图片信息，通过多种接口的方式传输给需要图片的服务程序

## 预期实现功能

### 多源

- ✅本地图片
- 本地视频
- 网络图片
- 网络视频
- ✅网络摄像头
- USB摄像头
- gRPC传入的图片

### 接口

- gRPC
- REST Api

## 第三方库安装

### HCNetSDK

[设备网络SDK](https://open.hikvision.com/download/5cda567cf47ae80dd41a54b3?type=10&id=5cda5902f47ae80dd41a54b7)

[播放库SDK](https://open.hikvision.com/download/5cda567cf47ae80dd41a54b3?type=10&id=f11dc00afd8342ac8996493490d77f57)

下载Linux64版本

将两个包中的头文件放在 ./thirdparty/HCNetSDK/include

将两个包中的库文件放在 ./thirdparty/HCNetSDK/lib

库文件中HCNetSDKCom文件夹也放在 ./thirdparty/HCNetSDK/lib 不需要将其中文件合并进 ./thirdparty/HCNetSDK/lib
