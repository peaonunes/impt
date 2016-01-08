#ifndef LZ78_H
#define LZ78_H

#include <string>

std::string lz78_encode (char* text, int text_lenght, int* code_length);
std::string lz78_decode (std::string code, int code_length);

#endif