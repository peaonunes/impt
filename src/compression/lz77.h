#ifndef LZ77_H
#define LZ77_H

#include <cstdint>

uint8_t* lz77_encode(char* txt, int txtlen, int Ls, int Ll, uint32_t* code_length);
void lz77_decode(uint8_t *code, int codelen, int Ls, int Ll, char* txt);

#endif
