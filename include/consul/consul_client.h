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
#include "consul/server_info.h"

class ConsulClient {
public:
    ConsulClient();
    // ~ConsulClient();
    ConsulClient& setConsulAddress(std::string);
    ConsulClient& setConsulPort(std::string);

    bool registerService(const ServerInfo& serverInfo);
    bool deregisterService();
    bool discoverServices(std::string serviceName, std::vector<std::pair<std::string, std::string>>& services);
private:
    std::string consulAddress;
    std::string consulPort;
};

#endif /* _CONSUL_CLIENT_H_ */
