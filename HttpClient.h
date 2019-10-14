#include <iostream>

#include "SocketWrapper.h"
#include "url.h"

#include "main.h"

#ifndef HTTP_CLIENT_HTTPCLIENT_H
#define HTTP_CLIENT_HTTPCLIENT_H

class HttpClient {
    static std::string get_request_from_url(const Url& url);
    static int get_content_length(const std::string& line);
public:
    static int try_send_http(SocketWrapper& socket, const Url& url);
};

#endif //HTTP_CLIENT_HTTPCLIENT_H
