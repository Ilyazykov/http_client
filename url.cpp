#include "url.h"

Url::Url(const std::string& url) {
    int host_begin = 0;
    int host_end = 0;

    for (int i = 0; i < url.size(); ++i) {
        if (url[i] == ':') {
            i += 3;
            host_begin = i;
        }
        else if (url[i] == '/') {
            host_end = i;
            break;
        }
    }

    if (host_end == 0) {
        host_end = url.size();
    }

    // TODO: add https and others maybe
    if ((host_begin > 0) && (url.substr(0, host_begin) != "http://")) {
        throw std::runtime_error("ERROR: bad request. Url should be 'http://'");
    }

    hostname = url.substr(host_begin, host_end - host_begin);

    if (url.size() == host_end) {
        address = '/';
    } else {
        address = url.substr(host_end, url.size() - host_end);
    }
}

std::string Url::get_hostname() const {
    return hostname;
}

std::string Url::get_address() const {
    return address;
}