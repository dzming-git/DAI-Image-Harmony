#include "image_loaders/image_loader_factory.h"
#include "image_loaders/local_image/local_image_loader.h"
#include "image_loaders/webcamera_hikvision/webcamera_hikvision.h"

std::unordered_map<std::string, ImageLoaderFactory::SourceType> ImageLoaderFactory::sourceTypeMap = {
        {"hikvision", ImageLoaderFactory::SourceType::WebCameraHikvision},
        {"local image", ImageLoaderFactory::SourceType::LocalImage}
};

ImageLoaderBase *ImageLoaderFactory::createImageLoader(ImageLoaderFactory::SourceType type) {
    switch (type) {
    case ImageLoaderFactory::SourceType::LocalImage:
        return new LocalImageLoader();
    case ImageLoaderFactory::SourceType::WebCameraHikvision:
        return new WebcameraHikvisionLoader();
    }
    return nullptr;
}
