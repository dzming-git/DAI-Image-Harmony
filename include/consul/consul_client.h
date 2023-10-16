/*****************************************************************************
*  Copyright © 2023 - 2023 dzming.                                           *
*                                                                            *
*  @file     consul_client.h                                                 *
*  @brief    Consul Client                                                   *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark  :                                                                 *
*****************************************************************************/

#ifndef _CONSUL_CLIENT_H_
#define _CONSUL_CLIENT_H_

#include <vector>
#include <string>

class ConsulClient {
public:
    ConsulClient();
    // ~ConsulClient();
    struct ServiceCheck;
    ConsulClient& setConsulAddress(std::string);
    ConsulClient& setConsulPort(std::string);
    ConsulClient& setServiceId(std::string);
    ConsulClient& setServiceName(std::string);    
    ConsulClient& setServiceAddress(std::string);
    ConsulClient& setServicePort(std::string);
    ConsulClient& setServiceTags(std::vector<std::string>);
    ConsulClient& setServiceCheck(ServiceCheck*);

    bool registerService();
    bool deregisterService();
private:
    std::string consulAddress;
    std::string consulPort;
    std::string serviceId;
    std::string serviceName;
    std::string serviceAddress;
    std::string servicePort;
    std::vector<std::string> serviceTags;
    ServiceCheck* serviceCheck;
};

struct ConsulClient::ServiceCheck {
    ServiceCheck();

    std::string protocol;
    std::string url;
    std::string status;
    int intervalMs;
    int timeoutMs;
};

#endif /* _CONSUL_CLIENT_H_ */
