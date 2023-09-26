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

#ifndef _HTTP_RESPONSE_HANDLER_BASE_H_
#define _HTTP_RESPONSE_HANDLER_BASE_H_

#include <string>

class HttpResponseHandlerBase {
public:
    HttpResponseHandlerBase();
    virtual std::string response(const std::string&) = 0;
};

#endif /* _HTTP_RESPONSE_HANDLER_BASE_H_ */
