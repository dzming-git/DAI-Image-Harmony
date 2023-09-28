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
#include "grpc/serviecs/img_trans/img_trans.h"

int main(int argc, char** argv) {
    GRPCServer::GRPCServerBuilder builder;
    ImgTransService imgTransService;
    builder.setHost("0.0.0.0")
           .setEpollCount(4, 8)
           .setMaxSendBytes(1024 * 1024 * 1024)
           .addService(&imgTransService);
    auto server = builder.build();
    server->start();
    return 0;
}

#endif