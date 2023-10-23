/*****************************************************************************
*  Copyright © 2023 - 2023 dzming  dzm_work@163.com.                         *
*                                                                            *
*  @file     http_response_handler_base.h                                    *
*  @brief    handle http response                                            *
*  @author   dzming                                                          *
*  @email    dzm_work@163.com                                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*****************************************************************************/

#ifndef _HTTP_RESPONSE_HANDLER_TEST_H_
#define _HTTP_RESPONSE_HANDLER_TEST_H_

#include <string>
#include "http_server/http_response_handler/http_response_handler_base.h"
#include "image_loaders/image_loader_factory.h"

class HttpResponseHandlerTset: public HttpResponseHandlerBase {
public:
    HttpResponseHandlerTset();
    virtual std::string response(const std::string&);
private:
    ImageLoaderBase* imgLoader;
};

#endif /* _HTTP_RESPONSE_HANDLER_TEST_H_ */
