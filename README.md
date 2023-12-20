# Image Harmony

DAI/集成层/Image Harmony

使用适配器模式+迭代器模式，处理多源数据，输出图片信息，通过多种接口的方式传输给需要图片的服务程序

## 预期实现功能

### 多源

- ✅本地图片
- ✅本地视频
- 网络图片
- ✅网络视频
- ✅网络摄像头
- USB摄像头
- gRPC传入的图片

### 接口

- ✅gRPC
- REST Api

## 环境配置

### 基础环境

[dzming-git/hard_decoding_install_scripts: 一键安装cuda+ffmpeg+opencv硬解码所需环境 (github.com)](https://github.com/dzming-git/hard_decoding_install_scripts)

### 其他环境

```bash
bash ./scrtpts/install_grpc.sh
bash ./scrtpts/install_libhv.sh
bash ./scrtpts/install_yaml-cpp.sh
```

## 第三方库安装

### HCNetSDK

[设备网络SDK](https://open.hikvision.com/download/5cda567cf47ae80dd41a54b3?type=10&id=5cda5902f47ae80dd41a54b7)

[播放库SDK](https://open.hikvision.com/download/5cda567cf47ae80dd41a54b3?type=10&id=f11dc00afd8342ac8996493490d77f57)

下载Linux64版本

将两个包中的头文件放在 `./src/image_loaders/webcamera_hikvision/HCNetSDK/include`

将两个包中的库文件放在 `./src/image_loaders/webcamera_hikvision/HCNetSDK/lib`

库文件中HCNetSDKCom文件夹也放在 `./src/image_loaders/webcamera_hikvision/HCNetSDK/lib` 不需要将其中文件合并进 `./src/image_loaders/webcamera_hikvision/HCNetSDK/lib`
