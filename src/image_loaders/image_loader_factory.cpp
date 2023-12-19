#include "image_loaders/image_loader_factory.h"
#include "image_loaders/local_image/local_image_loader.h"
#include "image_loaders/webcamera_hikvision/webcamera_hikvision.h"
#include <vector>
#include <algorithm>
#include <iostream>

/*
 * 编辑距离 计算字符串的相似度
*/
int minDistance(std::string word1, std::string word2) {
    int n1 = word1.size();
    int n2 = word2.size();
    std::vector<std::vector<int>> dp(n1 + 1, std::vector<int>(n2 + 1, 0));
    for (int i = 1; i <= n1; ++i) {
        dp[i][0] = i;
    }
    for (int j = 1; j <= n2; ++j) {
        dp[0][j] = j;
    }
    for (int i = 1; i <= n1; ++i) {
        for (int j = 1; j <= n2; ++j) {
            if (word1[i - 1] == word2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else {
                dp[i][j] = std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]}) + 1;
            }
        }
    }
    return dp[n1][n2];
}

std::unordered_map<std::string, ImageLoaderFactory::SourceType> ImageLoaderFactory::sourceTypeMap = {
        {"hikvision", ImageLoaderFactory::SourceType::WebCameraHikvision},
        {"local image", ImageLoaderFactory::SourceType::LocalImage},
};

std::string ImageLoaderFactory::getMostSimilarSourceType(std::string typeString) {
    int minDist = INT_MAX;
    std::string mostSimilarType;

    // 遍历源类型映射表
    for (const auto& pair : sourceTypeMap) {
        std::string sourceType = pair.first;
        
        // 计算编辑距离
        int dist = minDistance(typeString, sourceType);
        
        // 更新最小距离和最相似的类型
        if (dist < minDist) {
            minDist = dist;
            mostSimilarType = sourceType;
        }
    }

    std::cout << "Intelligent matching " << typeString << " -> " << mostSimilarType << std::endl;
    // 返回最相似的源类型
    return mostSimilarType;
}

ImageLoaderBase *ImageLoaderFactory::createImageLoader(ImageLoaderFactory::SourceType type) {
    switch (type) {
    case ImageLoaderFactory::SourceType::LocalImage:
        return new LocalImageLoader();
    case ImageLoaderFactory::SourceType::WebCameraHikvision:
        return new HikvisionVideoReader();
    }
    return nullptr;
}
