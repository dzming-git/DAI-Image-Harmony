#include "http_server/http_response_handler/http_response_handler_test.h"
#include <opencv2/opencv.hpp>

inline std::string base64_encode(const unsigned char* data, size_t length) {
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    encoded.reserve(((length + 2) / 3) * 4);

    for (size_t i = 0; i < length; i += 3) {
        unsigned char b1 = data[i];
        unsigned char b2 = (i + 1 < length) ? data[i + 1] : 0;
        unsigned char b3 = (i + 2 < length) ? data[i + 2] : 0;

        unsigned char b1_base64 = (b1 & 0xFC) >> 2;
        unsigned char b2_base64 = ((b1 & 0x03) << 4) | ((b2 & 0xF0) >> 4);
        unsigned char b3_base64 = ((b2 & 0x0F) << 2) | ((b3 & 0xC0) >> 6);
        unsigned char b4_base64 = (b3 & 0x3F);

        encoded.push_back(base64_chars[b1_base64]);
        encoded.push_back(base64_chars[b2_base64]);
        encoded.push_back((i + 1 < length) ? base64_chars[b3_base64] : '=');
        encoded.push_back((i + 2 < length) ? base64_chars[b4_base64] : '=');
    }

    return encoded;
}

inline std::string matToBase64(const cv::Mat& image) {
    std::vector<uchar> buffer;
    cv::imencode(".jpg", image, buffer);
    
    std::string data(reinterpret_cast<char*>(buffer.data()), buffer.size());
    std::ostringstream os;
    os << "data:image/jpeg;base64,";

    std::string encoded = base64_encode(reinterpret_cast<const unsigned char*>(data.c_str()), data.length());
    os << encoded;

    return os.str();
}

HttpResponseHandlerTset::HttpResponseHandlerTset() {
    imgLoader = ImageLoaderFactory::createImageLoader(ImageLoaderFactory::SourceType::LocalImage);
    imgLoader->setArgument("ImagePaths", "/workspace/tests/images/lena.png");
}

std::string HttpResponseHandlerTset::response(const std::string &) {
    if (!imgLoader->hasNext()) {
        return "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n404 Not Found";
    }

    std::string encodedImage = matToBase64(imgLoader->next());

    std::string html = "<!DOCTYPE html>\n"
                       "<html>\n"
                       "<head>\n"
                       "<title>显示图片</title>\n"
                       "</head>\n"
                       "<body>\n"
                       "<img src=\"" + encodedImage + "\" alt=\"图片\">\n"
                       "</body>\n"
                       "</html>";

    return "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + html;
}
