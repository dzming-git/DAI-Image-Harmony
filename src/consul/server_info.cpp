#include "consul/server_info.h"

ServerInfo::ServerInfo() {
    serviceCheck = nullptr;
}

ServerInfo::~ServerInfo() {
}

ServerInfo::ServiceCheck::ServiceCheck() {
    protocol = "TCP";
    url = "";
    status = "passing";
    intervalMs = 10000;
    timeoutMs = 3000;
}

ServerInfo& ServerInfo::setServiceId(std::string id) {
    serviceId = id;
    return *this;
}

std::string ServerInfo::getServiceId() const {
    return serviceId;
}

ServerInfo& ServerInfo::setServiceName(std::string name) {
    serviceName = name;
    return *this;
}

std::string ServerInfo::getServiceName() const {
    return serviceName;
}

ServerInfo& ServerInfo::setServiceAddress(std::string address) {
    serviceAddress = address;
    return *this;
}

std::string ServerInfo::getServiceAddress() const {
    return serviceAddress;
}

ServerInfo& ServerInfo::setServicePort(std::string port) {
    servicePort = port;
    return *this;
}

std::string ServerInfo::getServicePort() const {
    return servicePort;
}

ServerInfo& ServerInfo::setServiceTags(std::vector<std::string> tags) {
    serviceTags = tags;
    return *this;
}

std::vector<std::string> ServerInfo::getServiceTags() const {
    return serviceTags;
}

ServerInfo& ServerInfo::setServiceCheck(ServiceCheck* check) {
    serviceCheck = check;
    return *this;
}

ServerInfo::ServiceCheck* ServerInfo::getServiceCheck() const {
    return serviceCheck;
}