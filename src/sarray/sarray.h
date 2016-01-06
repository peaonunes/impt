#ifndef SARRAY_H
#define SARRAY_H

#include <cstdint>

// text: O texto (inteiro) a ser indexado
// text_length: Comprimento em caracteres do texto
// sarray, Llcp, Rlcp: ponteiros para inteiro passados por referência.
// sarray passará a ser o array de sufixos,
// e L(R)lcp serão os arrays com os maiores prefixos comuns em relação à busca binária
void build_sarray_LRlcp(char* text, int text_length, int** sarray, int** Llcp, int** Rlcp);
// matches_start(end): referência para os inteiros que guardarão os índices
// onde começam e terminam as ocorrências no array de sufixos
// text: O texto a ser buscado
// txtlen: Comprimento do texto
// pattern: O padrão a ser encontrado no texto
// patlen: Comprimento do padrão
// sarray: array de sufixos do texto
// Llcp, Rlcp: arrays com os prefixos comuns da busca binária
void find_occurrences(int* matches_start, int* matches_end, char* text, int txtlen, char* pattern, int patlen, int* sarray, int* Llcp, int* Rlcp);
// Função utilizada para transformar os arrays em arrays de bytes
// para compressão e serialização
char* get_bytes_from_array(int* array, uint32_t arraylen);

#endif
