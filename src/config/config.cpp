#include "config/config.h"
#include "yaml-cpp/yaml.h"
#include "utils/log.h"
#include <iostream>

Config* Config::instance = nullptr;
pthread_mutex_t Config::lock;

Config::Config() {
    std::cout << "load config: " << CONFIG_PATH << std::endl;
    YAML::Node root;
    try {
        root = YAML::LoadFile(CONFIG_PATH);
    } catch (YAML::ParserException &ex) {
        LOG("config parse failed: %s\n", ex.what());
        exit(-1);
    } catch (YAML::BadFile &ex) {
        LOG("config parse failed: %s\n", ex.what());
        exit(-1);
    }
    
    // service node:
    auto nodeService = root["service"];
    if (!nodeService.IsDefined()) {
        LOG("Node [service] not found\n");
        return;
    }
    if (nodeService["name"].IsDefined()) {
        serviceName = nodeService["name"].as<std::string>();
        std::cout << "service name: " << serviceName << std::endl;
    }
    else {
        LOG("Node [service][name] not found\n");
        return;
    }
    if (nodeService["port"].IsDefined()) {
        servicePort = nodeService["port"].as<std::string>();
        std::cout << "service port: " << nodeService["port"].as<std::string>() << std::endl;
    }
    else {
        LOG("Node [service][port] not found\n");
        return;
    }
    if (nodeService["tags"].IsDefined()) {
        auto nodeServiceTags = nodeService["tags"];
        serviceTags.reserve(nodeServiceTags.size());
        for (auto tag : nodeServiceTags) {
            auto tagString = tag.as<std::string>();
            serviceTags.emplace_back(tagString);
            std::cout << "service tag: " << tagString << std::endl;
        }
    }
    else {
        LOG("Node [service][tags] not found\n");
    }
    // consul node:
    auto nodeConsul = root["consul"];
    bool nodeConsulDefined = true;
    if (!nodeConsul.IsDefined()) {
        LOG("Node [consul] not found\n");
        nodeConsulDefined = false;
    }
    if (nodeConsulDefined && nodeConsul["ip"].IsDefined()) {
        consulIp = nodeConsul["ip"].as<std::string>();
        std::cout << "consul ip: " << consulIp << std::endl;
    }
    else {
        LOG("Node [consul][ip] not found\n");
    }
    if (nodeConsulDefined && nodeConsul["port"].IsDefined()) {
        consulPort = nodeConsul["port"].as<std::string>();
        std::cout << "consul port: " << consulPort << std::endl;
    }
    else {
        LOG("Node [consul][port] not found\n");
    }

    // settings node:
    auto settingsConsul = root["settings"];
    bool nodeSettingsDefined = true;
    if (!settingsConsul.IsDefined()) {
        nodeSettingsDefined = false;
    }
    if (nodeSettingsDefined && settingsConsul["historyMaxSize"].IsDefined()) {
        historyMaxSize = settingsConsul["historyMaxSize"].as<int>();
        std::cout << "historyMaxSize: " << historyMaxSize << std::endl;
    }
    else {
        historyMaxSize = 100;
        std::cout << "historyMaxSize (default): " << historyMaxSize << std::endl;
    }
    if (nodeSettingsDefined && settingsConsul["imageLoaderTimeout"].IsDefined()) {
        imageLoaderTimeout = settingsConsul["imageLoaderTimeout"].as<int>();
        std::cout << "imageLoaderTimeout: " << imageLoaderTimeout << std::endl;
    }
    else {
        imageLoaderTimeout = 120;
        std::cout << "imageLoaderTimeout (default): " << imageLoaderTimeout << std::endl;
    }
    std::cout << std::endl;
}

Config* Config::getSingletonInstance() {
	if (nullptr == instance) {
		pthread_mutex_lock(&lock);
		if (nullptr == instance) {
			instance = new Config();
		}
        pthread_mutex_unlock(&lock);
	}
	return instance;
}

std::string Config::getServiceName() const {
    return serviceName;
}

std::string Config::getServicePort() const {
    return servicePort;
}

std::vector<std::string> Config::getServiceTags() const {
    return serviceTags;
}

std::string Config::getConsulIp() const {
    return consulIp;
}

std::string Config::getConsulPort() const {
    return consulPort;
}

int Config::getHistoryMaxSize() const {
    return historyMaxSize;
}

int Config::getImageLoaderTimeout() const {
    return imageLoaderTimeout;
}
