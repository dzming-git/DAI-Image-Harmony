#include "consul/consul_client.h"
#include "hv/requests.h"
#include "hv/json.hpp"
#include <string>

ConsulClient::ServiceCheck::ServiceCheck() {
    protocol = "TCP";
    url = "";
    status = "passing";
    intervalMs = 10000;
    timeoutMs = 3000;
}

ConsulClient::ConsulClient(): consulPort("8500"), serviceCheck(nullptr) {
    // 从环境变量中读取consul地址
    char* consulAddressPtr = std::getenv("CONSUL_ADDRESS");
    if (nullptr == consulAddressPtr) {
        consulAddress = "127.0.0.1"; 
    }
    else {
        consulAddress = consulAddressPtr; 
    }
}

ConsulClient& ConsulClient::setConsulAddress(std::string address) {
    consulAddress = address;
    return *this;
}

ConsulClient& ConsulClient::setConsulPort(std::string port) {
    consulPort = port;
    return *this;
}

ConsulClient& ConsulClient::setServiceId(std::string id) {
    serviceId = id;
    return *this;
}

ConsulClient& ConsulClient::setServiceName(std::string name) {
    serviceName = name;
    return *this;
}

ConsulClient& ConsulClient::setServiceAddress(std::string address) {
    serviceAddress = address;
    return *this;
}

ConsulClient& ConsulClient::setServicePort(std::string port) {
    servicePort = port;
    return *this;
}

ConsulClient& ConsulClient::setServiceTags(std::vector<std::string> tags) {
    serviceTags = tags;
    return *this;
}

ConsulClient& ConsulClient::setServiceCheck(ServiceCheck* check) {
    serviceCheck = check;
    return *this;
}

bool ConsulClient::registerService() {
    HttpRequest req;
    req.method = HTTP_PUT;
    req.url = "http://" + consulAddress + ":" + consulPort + "/v1/agent/service/register";
    req.content_type = APPLICATION_JSON;
    nlohmann::json serviceInfoJson;
    serviceInfoJson["Name"] = serviceName;
    serviceInfoJson["Address"] = serviceAddress;
    serviceInfoJson["Port"] = atoi(servicePort.data());
    nlohmann::json serviceCheckInfoJson;
    ServiceCheck check;
    if (nullptr == serviceCheck) {
        check.url = serviceAddress + ":" + servicePort;
        serviceCheck = &check;
    }
    serviceCheckInfoJson[serviceCheck->protocol] = serviceCheck->url;
    serviceCheckInfoJson["Interval"] = std::to_string(serviceCheck->intervalMs) + "ms";
    serviceCheckInfoJson["DeregisterCriticalServiceAfter"] = std::to_string(serviceCheck->intervalMs * 3) + "ms";
    serviceInfoJson["Check"] = serviceCheckInfoJson;
    req.body = serviceInfoJson.dump();
    std::cout << "PUT " << req.url << std::endl
              << req.body << std::endl;
    HttpResponse res;
    int ret = http_client_send(&req, &res);
    std::cout << res.body << std::endl;
    return ret;
}