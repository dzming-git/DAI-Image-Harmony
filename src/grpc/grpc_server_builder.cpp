#include "grpc/grpc_server.h"
#include "grpc/grpc_server_builder.h"

GRPCServer::GRPCServerBuilder::GRPCServerBuilder(): host("127.0.0.1"), port("5000") {
    serverBuilder = new grpc::ServerBuilder();
}

GRPCServer::GRPCServerBuilder::~GRPCServerBuilder() {
    if (serverBuilder) {
        delete serverBuilder;
    }
}

GRPCServer::GRPCServerBuilder& GRPCServer::GRPCServerBuilder::setHost(std::string host) {
    this->host = host;
    return *this;
}

std::string GRPCServer::GRPCServerBuilder::getHost() {
    return host;
}

GRPCServer::GRPCServerBuilder &GRPCServer::GRPCServerBuilder::setPort(std::string port) {
    this->port = port;
    return *this;
}

std::string GRPCServer::GRPCServerBuilder::getPort() {
    return port;
}

GRPCServer::GRPCServerBuilder &GRPCServer::GRPCServerBuilder::addService(grpc::Service * service) {
    serverBuilder->RegisterService(service);
    return *this;
}

GRPCServer::GRPCServerBuilder &GRPCServer::GRPCServerBuilder::setMaxSendBytes(int maxBytes) {
    serverBuilder->SetMaxSendMessageSize(maxBytes);
    serverBuilder->AddChannelArgument(GRPC_ARG_MAX_METADATA_SIZE, maxBytes);
    return *this;
}

GRPCServer::GRPCServerBuilder &GRPCServer::GRPCServerBuilder::setEpollCount(int minCnt, int maxCnt) {
    serverBuilder->SetSyncServerOption(grpc::ServerBuilder::MIN_POLLERS, minCnt);
    serverBuilder->SetSyncServerOption(grpc::ServerBuilder::MAX_POLLERS, maxCnt);
    return *this;
}

GRPCServer *GRPCServer::GRPCServerBuilder::build() {
    serverBuilder->AddListeningPort(host + ":" + port, grpc::InsecureServerCredentials());
    return new GRPCServer(this, serverBuilder);
}
