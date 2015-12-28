#ifndef SARRAY_H
#define SARRAY_H

// text: O texto (inteiro) a ser indexado
// text_length: Comprimento em caracteres do texto
// sarray, Llcp, Rlcp: ponteiros para inteiro passados por referência.
// sarray passará a ser o array de sufixos,
// e L(R)lcp serão os arrays com os maiores prefixos comuns em relação à busca binária
void build_sarray_LRlcp(char* text, int text_length, int** sarray, int** Llcp, int** Rlcp);

#endif
