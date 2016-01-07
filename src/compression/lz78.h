#ifndef LZ78_H
#define LZ78_H

#include <string>

uint8_t* lz78_encode (char* text, int text_length, uint32_t* code_length);
char* lz78_decode(uint8_t* text);

#endif