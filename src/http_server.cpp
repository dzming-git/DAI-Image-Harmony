#include "http_server.h"
#include "image_loader_factory.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <unistd.h>

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

inline std::string handle_request(const std::string& request) {
    // 在这里处理请求并返回响应
    ImageLoaderBase* imgLoader = ImageLoaderFactory::createImageLoader(ImageLoaderFactory::SourceType::LocalImage);
    imgLoader->setSource("../tests/images/lena.png");

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

HttpServer::Builder::Builder():host("0.0.0.0"), port(5000) {
}

std::string HttpServer::Builder::getHost() {
    return host;
}

uint16_t HttpServer::Builder::getPort() {
    return port;
}

void HttpServer::Builder::setHost(std::string host) {
    this->host = host;
}

void HttpServer::Builder::setPort(uint16_t port) {
    this->port = port;
}

HttpServer *HttpServer::Builder::build() {
    return new HttpServer(this);
}

std::string HttpServer::getHost() {
    return host;
}

uint16_t HttpServer::getPort() {
    return port;
}

void HttpServer::start() {
    int new_socket;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    while (true) {
        if ((new_socket = accept(fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        memset(buffer, 0, sizeof(buffer));
        read(new_socket, buffer, sizeof(buffer));

        std::string request(buffer);
        std::string response = handle_request(request);

        send(new_socket, response.c_str(), response.length(), 0);

        close(new_socket);
    }
}

HttpServer::HttpServer(HttpServer::Builder *builder):host(builder->getHost()), port(builder->getPort()) {
    // 创建套接字
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host.c_str());
    address.sin_port = htons(port);

    // 绑定端口
    if (bind(fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听连接
    if (listen(fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}
