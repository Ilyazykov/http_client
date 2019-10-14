#include "functions.h"

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