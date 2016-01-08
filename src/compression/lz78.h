#ifndef LZ78_H
#define LZ78_H

uint8_t* lz78_encode (char* text, uint32_t text_lenght, uint32_t* code_length);
char* lz78_decode (uint8_t* code, uint32_t code_length, uint32_t text_length);

#endif