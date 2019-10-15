#include "HttpClient.h"

std::string HttpClient::get_request_from_url() const {
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

void HttpClient::send_request() const {
    std::string request = get_request_from_url();
    socket.send_msg(request);
    std::cout << "HTTP request send. ";
}

// return content length
int HttpClient::recv_header() {
    std::string line = socket.recv_msg_line();
    std::cout << line << std::endl;

    int content_length = UNKNOWN_LENGTH;

    // TODO added only common responses, need to add others
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
                content_length = parse_content_length(line);
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
                url = Url(new_url_str);

                std::cout << "address: " << new_url_str << " [moving]" << std::endl;

                socket.restart(url.get_hostname());
                send_request();

                return recv_header();
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

                url = Url(url.get_hostname(), new_address);

                socket.restart();
                send_request();

                return recv_header();
            }
        }
    }
    else if (line == "HTTP/" + HTTP_VERSION + " 404 Not Found") {
        throw std::runtime_error("ERROR: 404: Not Found");
    }


    std::string content_length_str = "no data";
    if (content_length >= 0) {
        std::string content_length_str = std::to_string(content_length);
    }
    std::cout << "Length: " << content_length_str << std::endl;

    return content_length;
}

std::string HttpClient::recv_body(int length) {
    std::string result = "";

    if (length >= 0) {
        char *buf = new char[length + 1];
        result = socket.recv_msg_by_length(length);
    }

    else if (length == CHUNKED_MODE) {
        std::string chunk_size_str = socket.recv_msg_line();

        while (chunk_size_str != "0") {
            int chunk_size = hex_str_to_int(chunk_size_str);
            result += socket.recv_msg_by_length(chunk_size);

            socket.recv_msg_line();
            chunk_size_str = socket.recv_msg_line();
        }
    }
    return result;
}

std::string HttpClient::send_and_recv() {
    send_request();
    std::cout << "Waiting for response... ";

    int length = recv_header();
    std::string result = recv_body(length);

    return result;
}

int HttpClient::parse_content_length(const std::string& line) const {
    int result = 0;

    for (auto i : line) {
        if (i >= '0' and i <= '9') {
            result = result*10 + (i-'0');
        }
    }

    return result;
}
