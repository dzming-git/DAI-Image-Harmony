/*****************************************************************************
*  OpenST Basic tool library                                                 *
*  Copyright (C) 2014 Henry.Wen  renhuabest@163.com.                         *
*                                                                            *
*  This file is part of OST.                                                 *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License version 3 as         *
*  published by the Free Software Foundation.                                *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with OST. If not, see <http://www.gnu.org/licenses/>.               *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*  @file     http_server.h                                                   *
*  @brief    HTTP服务器                                                       *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*****************************************************************************/

#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

class HttpServer {
public:
    class Builder;
    std::string getHost();
    uint16_t getPort();
    void start();
private:
    HttpServer(HttpServer::Builder* builder);

    std::string host;
    uint16_t port;
    int fd; // 套接字
    struct sockaddr_in address;  // 地址
};

class HttpServer::Builder {
public:
    Builder();
    std::string getHost();
    uint16_t getPort();
    void setHost(std::string host);
    void setPort(uint16_t port);
    HttpServer* build();
private:
    std::string host;
    uint16_t port;
};

#endif /* _HTTP_SERVER_H_ */