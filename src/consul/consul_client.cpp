#include "consul/consul_client.h"
#include "hv/requests.h"
#include "hv/json.hpp"
#include <string>

ConsulClient::ConsulClient(): consulPort("8500") {
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

bool ConsulClient::registerService(const ServerInfo& serverInfo) {
    HttpRequest req;
    req.method = HTTP_PUT;
    req.url = "http://" + consulAddress + ":" + consulPort + "/v1/agent/service/register";
    req.content_type = APPLICATION_JSON;
    nlohmann::json serviceInfoJson;
    serviceInfoJson["ID"] = serverInfo.getServiceId();
    serviceInfoJson["Name"] = serverInfo.getServiceName();
    serviceInfoJson["Address"] = serverInfo.getServiceAddress();
    serviceInfoJson["Port"] = atoi(serverInfo.getServicePort().data());    
    nlohmann::json tagsJson = nlohmann::json::array();
    for (auto& tag : serverInfo.getServiceTags()) {
        tagsJson.push_back(tag);
    }
    serviceInfoJson["Tags"] = tagsJson;
    nlohmann::json serviceCheckInfoJson;
    ServerInfo::ServiceCheck* check = serverInfo.getServiceCheck();
    bool needDeleteCheckPtr = false;
    if (nullptr == check) {
        needDeleteCheckPtr = true;
        check = new ServerInfo::ServiceCheck();
        check->url = serverInfo.getServiceAddress() + ":" + serverInfo.getServicePort();
    }
    serviceCheckInfoJson[check->protocol] = check->url;
    serviceCheckInfoJson["Interval"] = std::to_string(check->intervalMs) + "ms";
    serviceCheckInfoJson["DeregisterCriticalServiceAfter"] = std::to_string(check->intervalMs * 3) + "ms";
    if (needDeleteCheckPtr) {
        delete check;
    }
    serviceInfoJson["Check"] = serviceCheckInfoJson;
    req.body = serviceInfoJson.dump();
    std::cout << "PUT " << req.url << std::endl
              << req.body << std::endl;
    HttpResponse res;
    int ret = http_client_send(&req, &res);
    std::cout << res.body << std::endl;
    return ret;
}
