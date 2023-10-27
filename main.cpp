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

#include "grpc/servers/grpc_server.h"
#include "grpc/servers/grpc_server_builder.h"
#include "grpc/servers/image_harmony/image_harmony_server.h"
#include "grpc/servers/service_coordinate/service_coordinate_server.h"
#include "consul/consul_client.h"
#include "consul/service_info.h"
#include "config/config.h"
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
    auto config = Config::getSingletonInstance();

    ConsulClient consul;
    consul
        .setConsulIp(config->getConsulIp())
        .setConsulPort(config->getConsulPort());
    ServiceInfo serviceInfo;
    std::string host = getPrivateIpLinux();
    std::string serviceName = config->getServiceName();
    std::string servicePort = config->getServicePort();
    serviceInfo
        .setServiceIp(host)
        .setServicePort(servicePort)
        .setServiceId(serviceName + "-" + host + ":" + servicePort)
        .setServiceName(serviceName)
        .setServiceTags(config->getServiceTags());
    consul.registerService(serviceInfo);
    GRPCServer::GRPCServerBuilder builder;
    ImageHarmonyServer imageHarmonyService;
    ServiceCoordinateServer taskCoordinateService;
    builder.setHost("0.0.0.0")
           .setEpollCount(4, 8)
           .setMaxSendBytes(1024 * 1024 * 1024)
           .addService(&imageHarmonyService)
           .addService(&taskCoordinateService);
    auto server = builder.build();
    server->start();
    return 0;
}

#endif