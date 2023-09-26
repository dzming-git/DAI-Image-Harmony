#include "http_server/http_server_builder.h"

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
