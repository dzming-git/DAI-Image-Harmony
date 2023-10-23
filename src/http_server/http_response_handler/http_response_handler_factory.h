/*****************************************************************************
*  Copyright © 2023 - 2023 dzming  dzm_work@163.com.                         *
*                                                                            *
*  @file     http_response_handler_factory.h                                 *
*  @brief    factory of the http response handler                            *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*****************************************************************************/

#ifndef _HTTP_RESPONSE_HANDLER_FACTORY_H_
#define _HTTP_RESPONSE_HANDLER_FACTORY_H_

#include <string>
#include "http_server/http_response_handler/http_response_handler_base.h"

class HttpResponseHandlerFactory {
public:
    static HttpResponseHandlerBase* createHttpResponseHandler(const std::string&);
};

#endif /* _HTTP_RESPONSE_HANDLER_FACTORY_H_ */
