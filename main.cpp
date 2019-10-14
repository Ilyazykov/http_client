#include <iostream>
#include <string>

#include "functions.h"
#include "SocketWrapper.h"
#include "url.h"
#include "HttpClient.h"


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
    SocketWrapper socket(hostname);
    std::cout << "connection established" << std::endl;

    int content_length = HttpClient::try_send_http(socket, hostname, address);

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