#include "image_loader_factory.h"
#include "local_image_loader.h"
#include "webcamera_hikvision.h"

ImageLoaderBase *ImageLoaderFactory::createImageLoader(ImageLoaderFactory::SourceType type) {
    switch (type) {
    case ImageLoaderFactory::SourceType::LocalImage:
        return new LocalImageLoader();
    case ImageLoaderFactory::SourceType::WebcameraHikvision:
        return new WebcameraHikvisionLoader();
    }
    return nullptr;
}
