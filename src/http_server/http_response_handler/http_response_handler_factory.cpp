#include "http_server/http_response_handler/http_response_handler_factory.h"
#include "http_server/http_response_handler/http_response_handler_test.h"

HttpResponseHandlerBase *HttpResponseHandlerFactory::createHttpResponseHandler(const std::string &type) {
    if ("/test" == type) {
        return new HttpResponseHandlerTset();
    }
    return nullptr;
}
