#ifndef LZ77_H
#define LZ77_H

#include <cstdint>

uint8_t* lz77_encode(char* txt, size_t txtlen, int Ls, int Ll, size_t* code_length);
void lz77_decode(uint8_t *code, size_t codelen, int Ls, int Ll, char* txt);

#endif
