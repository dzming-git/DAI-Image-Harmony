#include "grpc/grpc_server.h"
#include "grpc/grpc_server_builder.h"

GRPCServer::GRPCServer(GRPCServerBuilder* myBuilder, grpc::ServerBuilder* officialBuilder) {
    server = officialBuilder->BuildAndStart();
    std::cout << "Server listening on " << myBuilder->getHost() + ":" + myBuilder->getPort() << std::endl;
}

void GRPCServer::start() {
    server->Wait();
}
