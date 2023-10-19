#if 0
#include "http_server/http_server.h"
#include "http_server/http_server_builder.h"

int main() {
    auto builder = HttpServer::Builder();
    builder.setHost("0.0.0.0");
    builder.setPort(5000);
    auto server = builder.build();
    server->start();
}

#else

#include "grpc/grpc_server.h"
#include "grpc/grpc_server_builder.h"
#include "grpc/services/img_trans/img_trans.h"
#include "grpc/services/task_coordinate/task_coordinate.h"
#include "consul/consul_client.h"
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

std::string getPrivateIpLinux() {
    int ret = 0;
    struct ifaddrs * ifAddrStruct = NULL;
    void * tmpAddrPtr = NULL;
    ret = getifaddrs(&ifAddrStruct);
    if (0 != ret) {
        return "";
    }
    std::string ip;
    int padress_buf_len = INET_ADDRSTRLEN;
    char addressBuffer[INET6_ADDRSTRLEN] = {0};
    while (NULL != ifAddrStruct ) {
        if (AF_INET == ifAddrStruct->ifa_addr->sa_family ) {
            tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, padress_buf_len);
            ip = std::string(addressBuffer);
            if ("127.0.0.1" != ip) return ip;
            memset(addressBuffer, 0, padress_buf_len);
        } 
        ifAddrStruct = ifAddrStruct->ifa_next;
    }
    return "";
}

int main(int argc, char** argv) {
    ConsulClient consul;
    std::string host = getPrivateIpLinux();
    consul.setServiceAddress(host)
          .setServicePort("5000")
          .setServiceId("image harmony-" + host + ":5000")
          .setServiceName("image harmony")
          .setServiceTags({"image harmony"})
          .registerService();
    GRPCServer::GRPCServerBuilder builder;
    ImgTransService imgTransService;
    TaskCoordinateService taskCoordinateService;
    builder.setHost("0.0.0.0")
           .setEpollCount(4, 8)
           .setMaxSendBytes(1024 * 1024 * 1024)
           .addService(&imgTransService)
           .addService(&taskCoordinateService);
    auto server = builder.build();
    server->start();
    return 0;
}

#endif