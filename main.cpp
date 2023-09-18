#include "http_server.h"

int main() {
    auto builder = HttpServer::Builder();
    builder.setHost("0.0.0.0");
    builder.setPort(5000);
    auto server = builder.build();
    server->start();
}