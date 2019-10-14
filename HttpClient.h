#include <iostream>

#include "SocketWrapper.h"
#include "url.h"

#ifndef HTTP_CLIENT_HTTPCLIENT_H
#define HTTP_CLIENT_HTTPCLIENT_H

class HttpClient {
public:
    static int try_send_http(SocketWrapper& socket, const std::string& hostname, const std::string& address) {
        socket.send_msg(get_request_from_url(hostname, address));
        std::cout << "HTTP request send. Waiting for response... ";

        std::string line = socket.recv_msg_line();
        std::cout << line << std::endl;

        int content_length = UNKNOWN_LENGTH;

        if (line == "HTTP/" + HTTP_VERSION + " 200 OK") {
            while (!line.empty()) {
                line = socket.recv_msg_line();

//            std::cout << line << std::endl; // DEBUG

                std::size_t found = line.find(CHUNKED);
                if (found != std::string::npos) {
                    content_length = CHUNKED_MODE;
                }

                found = line.find(LENGTH_WORD);
                if (found != std::string::npos) {
                    content_length = get_content_length(line);
                }
            }
        }
        else if (line == "HTTP/" + HTTP_VERSION + " 301 Moved Permanently") {
            while (!line.empty()) {
                line = socket.recv_msg_line();
//            std::cout << line << std::endl; // DEBUG

                std::size_t found = line.find(LOCATION_WORD);
                if (found != std::string::npos) {
                    std::string new_url_str = line.substr(LOCATION_WORD.size(), line.size() - LOCATION_WORD.size());
                    Url new_url(new_url_str);

                    std::cout << "address: " << new_url_str << " [moving]" << std::endl;

                    socket.restart(new_url.get_hostname());
                    try_send_http(socket, new_url.get_hostname(), new_url.get_address());
                }
            }
        }
        else if (line == "HTTP/" + HTTP_VERSION + " 302 Moved Temporarily") {
            while (!line.empty()) {
                line = socket.recv_msg_line();

                std::size_t found = line.find(LOCATION_WORD);
                if (found != std::string::npos) {
                    std::string new_address = line.substr(LOCATION_WORD.size(), line.size() - LOCATION_WORD.size());
                    std::cout << "address: " << new_address << " [moving]" << std::endl;
                    std::cout << "reusing connection " << hostname << std::endl;
                    socket.restart();
                    try_send_http(socket, hostname, new_address);
                }
            }
        }
        else if (line == "HTTP/" + HTTP_VERSION + " 404 Not Found") {
            std::cout << "ERROR: 404: Not Found";
            exit(1);
        }

        return content_length;
    }
};

#endif //HTTP_CLIENT_HTTPCLIENT_H
