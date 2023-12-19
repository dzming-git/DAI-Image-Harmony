#include "utils/random_utils.h"

#include <random>
#include <chrono>

std::int64_t generateInt64Random() {
    auto time = std::chrono::system_clock::now();
    auto duration = time.time_since_epoch();
    auto seed = static_cast<unsigned int>(duration.count());
    std::mt19937_64 rng(seed);
    
    // 生成int64_t范围内的随机数
    std::uniform_int_distribution<int64_t> dist(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());
    return dist(rng);
}
