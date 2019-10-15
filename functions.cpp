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
            throw std::runtime_error("ERROR: wrong HEX number");
        }
    }

    return result;
}

void output(std::string content, std::string output_file) {
    std::cout << "saving to " << output_file << std::endl;

    std::ofstream out;
    out.open(output_file);
    if (out.is_open()) {
        out << content;
    }

    std::cout << output_file << " saved";
}