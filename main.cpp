#include <iostream>
#include <string>

#include "functions.h"
#include "SocketWrapper.h"
#include "url.h"
#include "HttpClient.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        throw std::runtime_error("ERROR: incorrect command line parameters."
                                 "Format of using is \"url\"");
    }

    std::cout << "recognizing " << argv[1] << "..." << std::endl;
    std::string url = argv[1];

    HttpClient httpClient(url);
    std::string content = httpClient.send_and_recv();

    std::string output_file = DEFAULT_OUTPUT_FILE; // TODO get output name from url
    output(content, output_file);


    return (EXIT_SUCCESS);
}