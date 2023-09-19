#include "image_loader_factory.h"
#include "local_image_loader.h"

ImageLoaderBase *ImageLoaderFactory::createImageLoader(ImageLoaderFactory::SourceType type) {
    switch (type) {
    case ImageLoaderFactory::SourceType::LocalImage:
        return new LocalImageLoader();
    }
    return nullptr;
}
