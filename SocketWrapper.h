#ifndef HTTP_CLIENT_SOCKETWRAPPER_H
#define HTTP_CLIENT_SOCKETWRAPPER_H

#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zconf.h>

#include "functions.h"

const int PORT = 80;

class SocketWrapper {
    int sock;
    std::string hostname;

    void start(const std::string& hostname);
public:
    explicit SocketWrapper(const std::string& hostname);
    ~SocketWrapper();

    void restart();
    void restart(std::string hostname);

    void send_msg(const std::string& message) const;

    std::string recv_msg_line() const;
    std::string recv_msg_by_length(int length) const;
};

#endif //HTTP_CLIENT_SOCKETWRAPPER_H
