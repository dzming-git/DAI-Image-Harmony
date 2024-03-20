#include "config/config.h"
#include "yaml-cpp/yaml.h"
#include "utils/log.h"
#include <iostream>

Config* Config::instance = nullptr;
std::mutex Config::lock;

Config::Config() {
    std::cout << "Loading config: " << CONFIG_PATH << std::endl;
    YAML::Node root;
    try {
        root = YAML::LoadFile(CONFIG_PATH);
    } catch (const YAML::ParserException &ex) {
        LOG("Config parse failed: %s\n", ex.what());
        exit(-1);
    } catch (const YAML::BadFile &ex) {
        LOG("Config parse failed: %s\n", ex.what());
        exit(-1);
    }

    // Services
    auto nodeServices = root["services"];
    if (!nodeServices.IsDefined()) {
        LOG("Node [services] not found\n");
        exit(-1);
    }
    for (const auto& nodeService : nodeServices) {
        ServiceConfig service;
        service.name = nodeService["name"].as<std::string>();
        service.type = nodeService["type"].as<std::string>();
        service.port = nodeService["port"].as<int>();
        auto tags = nodeService["tags"];
        for (const auto& tag : tags) {
            service.tags.emplace_back(tag.as<std::string>());
        }
        services.emplace_back(service);
    }

    // Consul
    auto nodeConsul = root["consul"];
    if (nodeConsul.IsDefined()) {
        consulIp = nodeConsul["ip"].as<std::string>();
        consulPort = nodeConsul["port"].as<int>();
    } else {
        LOG("Node [consul] not found\n");
        exit(-1);
    }

    // Settings
    auto nodeSettings = root["settings"];
    if (nodeSettings.IsDefined()) {
        historyMaxSize = nodeSettings["historyMaxSize"].as<int>(100);
        imageLoaderTimeout = nodeSettings["imageLoaderTimeout"].as<int>(120);
    } else {
        LOG("Node [settings] not found\n");
        exit(-1);
    }
}

Config* Config::getSingletonInstance() {
    std::lock_guard<std::mutex> guard(lock);
    if (instance == nullptr) {
        instance = new Config();
    }
    return instance;
}

std::vector<Config::ServiceConfig> Config::getServices() const {
    return services;
}

std::string Config::getConsulIp() const {
    return consulIp;
}

int Config::getConsulPort() const {
    return consulPort;
}

int Config::getHistoryMaxSize() const {
    return historyMaxSize;
}

int Config::getImageLoaderTimeout() const {
    return imageLoaderTimeout;
}