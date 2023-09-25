#include "image_loaders/image_loader_factory.h"
#include "image_loaders/local_image/local_image_loader.h"
#include "image_loaders/webcamera_hikvision/webcamera_hikvision.h"

enum ImageLoaderFactory::SourceType {
    LocalImage,
    WebcameraHikvision
};

ImageLoaderBase *ImageLoaderFactory::createImageLoader(ImageLoaderFactory::SourceType type) {
    switch (type) {
    case ImageLoaderFactory::SourceType::LocalImage:
        return new LocalImageLoader();
    case ImageLoaderFactory::SourceType::WebcameraHikvision:
        return new WebcameraHikvisionLoader();
    }
    return nullptr;
}
