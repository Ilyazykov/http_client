#include <iostream>
#include <string>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zconf.h>

const std::string OUTPUT_FILE = "index.html";
const std::string HTTP_VERSION = "1.1";
const std::string LENGTH_WORD = "Content-Length: ";
const std::string CHUNKED = "Transfer-Encoding: chunked";
const std::string LOCATION_WORD = "Location: ";
const int CHUNKED_MODE = -1;
const int UNKNOWN_LENGTH = -2;


const int PORT = 80;

int hex_str_to_int(std::string hex)
{
    int result = 0;
    for (auto letter : hex) {
        if (letter >= '0' && letter <= '9') {
            result = result * 16 + (letter - '0');
        }
        else if (letter >= 'a' && letter <= 'f') {
            result = result * 16 + (letter + 10 - 'a');
        }
        else if (letter >= 'A' && letter <= 'F') {
            result = result * 16 + (letter + 10 - 'A');
        }
        else {
            std::cout << "ERROR: wrong HEX number";
            exit(1);
        }
    }

    return result;
}

class raii_socket {
    int sock;
    std::string hostname;
public:
    explicit raii_socket(const std::string& hostname) {
        start(hostname);
    }

    ~raii_socket() {
        close(sock);
    }

    void start(const std::string& hostname) {
        this->hostname = hostname;

        hostent* raw_host;
        raw_host = gethostbyname(hostname.c_str());
        if (raw_host == nullptr) {
            std::cout << "ERROR: no such host";
            exit(0);
        }

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("socket");
            exit(1);
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        bcopy((char*)raw_host->h_addr, (char*)&addr.sin_addr, raw_host->h_length);
        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "connect error" << std::endl;
            exit(2);
        }
    }

    void restart() {
        close(sock);
        start(this->hostname);
    }

    void restart(std::string hostname) {
        close(sock);
        start(hostname);
    }

    void send_msg(const std::string& message) const {
        send(sock, message.c_str(), message.size(), 0);
    }

    std::string recv_msg_line() const {
        std::string result;

        char buf[1];
        while (true) {
            recv(sock, buf, sizeof(buf), 0);

            if (buf[0] == '\n') {
                break;
            }

            if (buf[0] != '\r') {
                result += buf[0];
            }
        }

        return result;
    }

    std::string recv_msg_by_length(int length) const {
        std::string result = "";
        if (length >= 0) {
            char *buf = new char[length + 1];
            recv(sock, buf, length, 0);

            return std::string(buf);
        }
        else if (length == CHUNKED_MODE) {
            std::string chunk_size_str = recv_msg_line();

            while (chunk_size_str != "0") {
                int chunk_size = hex_str_to_int(chunk_size_str);
                while (chunk_size > 0) {
                    char *buf = new char[chunk_size];
                    int get_size = recv(sock, buf, chunk_size, 0);

                    chunk_size -= get_size;
                    result += buf;
                }
                recv_msg_line();
                chunk_size_str = recv_msg_line();
            }
        }

        return result;
    }


};

std::string get_request_from_url(const std::string& hostname, const std::string& address) {
    std::string get_query = "GET " + address + " HTTP/" + HTTP_VERSION + "\r\nHost: " + hostname;

    std::string additional_info = "\r\n"
          "User-Agent: Mozilla/5.0\r\n"
          "Accept: text/html\r\n"
          "Accept-Language: ru,en-us;q=0.7,en;q=0.3\r\n"
          "Accept-Charset: windows-1251,utf-8;q=0.7,*;q=0.7\r\n"
          "Connection: keep-alive\r\n\r\n";

    return get_query + additional_info;
}

int get_content_length(const std::string& line) {
    int result = 0;

    for (auto i : line) {
        if (i >= '0' and i <= '9') {
            result = result*10 + (i-'0');
        }
    }

    return result;
}

void output(std::string content) {
    std::ofstream out;
    out.open(OUTPUT_FILE);
    if (out.is_open()) {
        out << content;
    }
}

class Url {
    std::string hostname;
    std::string address;
public:
    Url(const std::string& url) {
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

    std::string get_hostname() {
        return hostname;
    }

    std::string get_address() {
        return address;
    }

};


int try_send_http(raii_socket& socket, const std::string& hostname, const std::string& address) {
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
        // TODO Location: http://www.google.com/ when google.com
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


int main(int argc, char *argv[]) {
    if (argc < 2) {
        throw std::runtime_error("ERROR: incorrect command line parameters."
                                 "Format of using is \"url\"");
    }

    std::cout << "recognizing " << argv[1] << "..." << std::endl;
    Url url(argv[1]);

    std::string hostname = url.get_hostname();
    std::string address = url.get_address();

    std::cout << "connecting " << hostname << " ...";
    raii_socket socket(hostname);
    std::cout << "connection established" << std::endl;

    int content_length = try_send_http(socket, hostname, address);

    std::string content_length_str = "no data";
    if (content_length >= 0) {
        std::string content_length_str = std::to_string(content_length);
    }
    std::cout << "Length: " << content_length_str << std::endl;

    std::string content = socket.recv_msg_by_length(content_length);
    std::cout << "saving to " << OUTPUT_FILE << std::endl;
    output(content);
    std::cout << OUTPUT_FILE << " saved";

    return (EXIT_SUCCESS);
}