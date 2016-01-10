#ifndef LZ78_H
#define LZ78_H

#include <cstdint>

uint8_t* lz78_encode (char* text, size_t text_lenght, size_t* code_length);
char* lz78_decode (uint8_t* code, size_t code_length, size_t text_length);

#endif
