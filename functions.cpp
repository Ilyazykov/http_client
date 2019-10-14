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

void output(std::string content) {
    std::ofstream out;
    out.open(OUTPUT_FILE);
    if (out.is_open()) {
        out << content;
    }
}