#include "HttpClient.h"

std::string HttpClient::get_request_from_url(const Url& url) {
    std::string hostname = url.get_hostname();
    std::string address = url.get_address();
    std::string get_query = "GET " + address + " HTTP/" + HTTP_VERSION + "\r\nHost: " + hostname;

    std::string additional_info = "\r\n"
                                  "Accept: text/html\r\n"
                                  "Accept-Language: ru,en-us;q=0.7,en;q=0.3\r\n"
                                  "Accept-Charset: windows-1251,utf-8;q=0.7,*;q=0.7\r\n"
                                  "Connection: keep-alive\r\n\r\n";

    return get_query + additional_info;
}

int HttpClient::try_send_http(SocketWrapper& socket, const Url& url) {
    socket.send_msg(get_request_from_url(url));
    std::cout << "HTTP request send. Waiting for response... ";

    std::string line = socket.recv_msg_line();
    std::cout << line << std::endl;

    int content_length = UNKNOWN_LENGTH;

    if (line == "HTTP/" + HTTP_VERSION + " 200 OK") {
        while (!line.empty()) {
            line = socket.recv_msg_line();
#if DEBUG_MODE
            std::cout << "DEBUG: " << line << std::endl;
#endif
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
#if DEBUG_MODE
            std::cout << "DEBUG: " << line << std::endl;
#endif
            std::size_t found = line.find(LOCATION_WORD);
            if (found != std::string::npos) {
                std::string new_url_str = line.substr(LOCATION_WORD.size(), line.size() - LOCATION_WORD.size());
                Url new_url(new_url_str);

                std::cout << "address: " << new_url_str << " [moving]" << std::endl;

                socket.restart(new_url.get_hostname());
                try_send_http(socket, new_url);
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
                std::cout << "reusing connection " << url.get_hostname() << std::endl;
                socket.restart();
                try_send_http(socket, Url(url.get_hostname(), new_address));
            }
        }
    }
    else if (line == "HTTP/" + HTTP_VERSION + " 404 Not Found") {
        throw std::runtime_error("ERROR: 404: Not Found");
    }

    return content_length;
}

int HttpClient::get_content_length(const std::string& line) {
    int result = 0;

    for (auto i : line) {
        if (i >= '0' and i <= '9') {
            result = result*10 + (i-'0');
        }
    }

    return result;
}