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

#include <mutex>
#include <vector>

const std::string CONFIG_PATH = "./.config.yml";

class Config {
public:
    static Config* getSingletonInstance();

    // service
    std::string getServiceName() const;
    std::string getServicePort() const;
    std::vector<std::string> getServiceTags() const;

    // consul
    std::string getConsulIp() const;
    std::string getConsulPort() const;

    // settings
    int getHistoryMaxSize() const;

private:
    Config();

    std::string serviceName;
    std::string servicePort;
    std::vector<std::string> serviceTags;

    std::string consulIp;
    std::string consulPort;

    int historyMaxSize;

    static Config* instance;
    static pthread_mutex_t lock;
};

#endif /* _CONFIG_H_ */
