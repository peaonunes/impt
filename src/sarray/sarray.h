#ifndef SARRAY_H
#define SARRAY_H

// text: O texto (inteiro) a ser indexado
// text_length: Comprimento em caracteres do texto
// sarray, Llcp, Rlcp: ponteiros para inteiro passados por referência.
// sarray passará a ser o array de sufixos,
// e L(R)lcp serão os arrays com os maiores prefixos comuns em relação à busca binária
void build_sarray_LRlcp(char* text, int text_length, int** sarray, int** Llcp, int** Rlcp);
// matches: referência para o array onde vão ser armazenadas as ocorrências do padrão
// text: O texto a ser buscado
// txtlen: Comprimento do texto
// pattern: O padrão a ser encontrado no texto
// patlen: Comprimento do padrão
// sarray: array de sufixos do texto
// Llcp, Rlcp: arrays com os prefixos comuns da busca binária
// Retorna a quantidade de ocorrências encontradas
int find_occurrences(int** matches, char* text, int txtlen, char* pattern, int patlen, int* sarray, int* Llcp, int* Rlcp);

#endif
