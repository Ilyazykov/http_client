#include "SocketWrapper.h"

SocketWrapper::SocketWrapper(const std::string& hostname) {
    start(hostname);
}

SocketWrapper::~SocketWrapper() {
    close(sock);
}

void SocketWrapper::start(const std::string& hostname) {
    std::cout << "connecting " << hostname << " ...";

    this->hostname = hostname;

    hostent* raw_host;
    raw_host = gethostbyname(hostname.c_str());
    if (raw_host == nullptr) {
        throw std::runtime_error("ERROR: no such host");
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("ERROR: socket doesn't work");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    bcopy((char*)raw_host->h_addr, (char*)&addr.sin_addr, raw_host->h_length);
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("ERROR: connect error");
    }

    std::cout << "connection established" << std::endl;
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

    while (length > 0) {
        char *buf = new char[length];
        int get_size = recv(sock, buf, length, 0);

        length -= get_size;
        result += buf;
    }

    return result;
}