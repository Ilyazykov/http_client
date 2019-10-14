#include <iostream>

#include "SocketWrapper.h"
#include "url.h"

#ifndef HTTP_CLIENT_HTTPCLIENT_H
#define HTTP_CLIENT_HTTPCLIENT_H

class HttpClient {
    static std::string get_request_from_url(const std::string& hostname, const std::string& address);
    static int get_content_length(const std::string& line);
public:
    static int try_send_http(SocketWrapper& socket, const std::string& hostname, const std::string& address);
};

#endif //HTTP_CLIENT_HTTPCLIENT_H
