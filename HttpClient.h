#ifndef HTTP_CLIENT_HTTPCLIENT_H
#define HTTP_CLIENT_HTTPCLIENT_H

#include <iostream>

#include "SocketWrapper.h"
#include "url.h"

#include "main.h"

class HttpClient {
    Url url;
    SocketWrapper socket;

    std::string get_request_from_url() const;
    int parse_content_length(const std::string& line) const;

    void send_request() const;
    int recv_header(); // return content length
    std::string recv_body(int length);

public:
    HttpClient(std::string url) : url(url), socket(this->url.get_hostname()) {}
    std::string send_and_recv();
    std::string get_output_filename();
};

#endif //HTTP_CLIENT_HTTPCLIENT_H
