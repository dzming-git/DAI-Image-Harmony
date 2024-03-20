/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     config.h                                                        *
*  @brief    config                                                          *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :                                                                 *
*****************************************************************************/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "config.h"
#include "yaml-cpp/yaml.h"
#include "utils/log.h"
#include <iostream>
#include <string>
#include <vector>
#include <mutex>

const std::string CONFIG_PATH = "./.config.yml";

class Config {
public:
    struct ServiceConfig {
        std::string name;
        std::string type;
        int port;
        std::vector<std::string> tags;
    };

    static Config* getSingletonInstance();

    // Services
    std::vector<ServiceConfig> getServices() const;

    // Consul
    std::string getConsulIp() const;
    int getConsulPort() const;

    // Settings
    int getHistoryMaxSize() const;
    int getImageLoaderTimeout() const;

private:
    Config();

    std::vector<ServiceConfig> services;
    std::string consulIp;
    int consulPort;
    int historyMaxSize;
    int imageLoaderTimeout;

    static Config* instance;
    static std::mutex lock;
};

#endif /* _CONFIG_H_ */
