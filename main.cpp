#include "grpc/servers/grpc_server.h"
#include "grpc/servers/grpc_server_builder.h"
#include "http_server/http_server.h"
#include "http_server/http_server_builder.h"
#include "grpc/servers/image_harmony/image_harmony_server.h"
#include "grpc/servers/service_coordinator/service_coordinator_server.h"
#include "consul/consul_client.h"
#include "consul/service_info.h"
#include "config/config.h"
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

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
    
    std::string host = getPrivateIpLinux();

    auto services = config->getServices();
    int grpcPort = 0;
    int httpPort = 0;
    for (const auto& service : services) {
        ServiceInfo serviceInfo;
        serviceInfo
            .setServiceIp(host)
            .setServicePort(service.port)
            .setServiceId(service.name + "-" + service.type + "-" + host + ":" + std::to_string(service.port))
            .setServiceName(service.name)
            .setServiceTags(service.tags);
        consul.registerService(serviceInfo);
        if (service.type == "gRPC") {
            grpcPort = service.port;
        } else if (service.type == "HTTP") {
            httpPort = service.port;
        }
    }

    // grpc
    GRPCServer::GRPCServerBuilder grpcServerBuilder;
    ImageHarmonyServer imageHarmonyService;
    ServiceCoordinatorServer taskCoordinateService;
    grpcServerBuilder
        .setHost("0.0.0.0")
        .setPort(grpcPort)
        .setEpollCount(4, 8)
        .setMaxSendBytes(1024 * 1024 * 1024)
        .addService(&imageHarmonyService)
        .addService(&taskCoordinateService);
    auto grpcServer = grpcServerBuilder.build();
    grpcServer->start();

    // http
    HttpServer::Builder httpServerBuilder;
    httpServerBuilder
        .setHost("0.0.0.0")
        .setPort(httpPort);
    auto httpServer = httpServerBuilder.build();
    httpServer->start();
    std::thread grpcThread([=]() {
        grpcServer->start();
    });

    std::thread httpThread([=]() {
        httpServer->start();
    });
    // 等待服务线程完成
    if (grpcThread.joinable()) {
        grpcThread.join();
    }
    if (httpThread.joinable()) {
        httpThread.join();
    }
    return 0;
}
