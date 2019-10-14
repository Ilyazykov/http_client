#include <string>
#include <iostream>
#include <fstream>

#ifndef HTTP_CLIENT_FUNCTIONS_H
#define HTTP_CLIENT_FUNCTIONS_H

const std::string HTTP_VERSION = "1.1";
const std::string OUTPUT_FILE = "index.html";
const std::string LENGTH_WORD = "Content-Length: ";
const std::string CHUNKED = "Transfer-Encoding: chunked";
const std::string LOCATION_WORD = "Location: ";
const int CHUNKED_MODE = -1;
const int UNKNOWN_LENGTH = -2;

int hex_str_to_int(std::string hex);
std::string get_request_from_url(const std::string& hostname, const std::string& address);
int get_content_length(const std::string& line);
void output(std::string content);

#endif //HTTP_CLIENT_FUNCTIONS_H
