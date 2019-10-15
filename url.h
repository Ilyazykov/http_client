#ifndef HTTP_CLIENT_URL_H
#define HTTP_CLIENT_URL_H

#include <string>

#include "main.h"

class Url {
    std::string hostname;
    std::string address;
public:
    Url(const std::string& url);
    Url(const std::string& hostname, const std::string& address) : hostname(hostname), address(address) {}
    std::string get_hostname() const;
    std::string get_address() const;
};

#endif //HTTP_CLIENT_URL_H
