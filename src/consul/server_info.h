/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     server_info.h                                                   *
*  @brief    Service information for registration or discovery               *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :                                                                 *
*****************************************************************************/

#ifndef _SERVER_INFO_H_
#define _SERVER_INFO_H_

#include <vector>
#include <string>

class ServerInfo {
public:
    ServerInfo();
    ~ServerInfo();
    struct ServiceCheck;

    ServerInfo& setConsulPort(std::string port);
    std::string getConsulPort() const;

    ServerInfo& setServiceId(std::string id);
    std::string getServiceId() const;

    ServerInfo& setServiceName(std::string name);
    std::string getServiceName() const;

    ServerInfo& setServiceAddress(std::string address);
    std::string getServiceAddress() const;

    ServerInfo& setServicePort(std::string port);
    std::string getServicePort() const;

    ServerInfo& setServiceTags(std::vector<std::string> tags);
    std::vector<std::string> getServiceTags() const;

    ServerInfo& setServiceCheck(ServiceCheck* check);
    ServiceCheck* getServiceCheck() const;
private:
    std::string serviceId;
    std::string serviceName;
    std::string serviceAddress;
    std::string servicePort;
    std::vector<std::string> serviceTags;
    ServiceCheck* serviceCheck;
};

struct ServerInfo::ServiceCheck {
    ServiceCheck();

    std::string protocol;
    std::string url;
    std::string status;
    int intervalMs;
    int timeoutMs;
};

#endif /* _SERVER_INFO_H_ */
