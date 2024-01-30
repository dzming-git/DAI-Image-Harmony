#define WHERE std::string("[") + __FILE__ + " : " + std::to_string(__LINE__) + "]"
#define LOG(fmt, ...) printf("[%s : %d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
