#include <string>

#ifndef HTTP_CLIENT_URL_H
#define HTTP_CLIENT_URL_H

class Url {
    std::string hostname;
    std::string address;
public:
    Url(const std::string& url);
    std::string get_hostname();
    std::string get_address();

};

#endif //HTTP_CLIENT_URL_H
