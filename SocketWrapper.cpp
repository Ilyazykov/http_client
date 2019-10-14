#include "SocketWrapper.h"

SocketWrapper::SocketWrapper(const std::string& hostname) {
    start(hostname);
}

SocketWrapper::~SocketWrapper() {
    close(sock);
}

void SocketWrapper::start(const std::string& hostname) {
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

void SocketWrapper::restart() {
    close(sock);
    start(this->hostname);
}

void SocketWrapper::restart(std::string hostname) {
    close(sock);
    start(hostname);
}

void SocketWrapper::send_msg(const std::string& message) const {
    send(sock, message.c_str(), message.size(), 0);
}

std::string SocketWrapper::recv_msg_line() const {
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

std::string SocketWrapper::recv_msg_by_length(int length) const {
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