#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <algorithm>

#include "sarray.h"

typedef struct sarray_temp {
	int first_block_order;
	int second_block_order;
	int start_index;
} sarray_temp;

bool operator <(const sarray_temp &lh, const sarray_temp &rh) {
	if (lh.first_block_order == rh.first_block_order) {
		if (lh.second_block_order == rh.second_block_order) {
			if (lh.start_index == rh.start_index) {
				return false;
			}
			return lh.start_index < rh.start_index;
		}
		return lh.second_block_order < rh.second_block_order;
	}
	return lh.first_block_order < rh.first_block_order;
}

// Aqui, first_block_order é usado pra guardar a letra apenas,
// de modo a reusar a struct sarray_temp.
// O algoritmo de ordenação usado aqui é o bucket sort.
sarray_temp* sort_letter_occurrences(sarray_temp* stemp, int len) {
	int buckets[256];
	int array_size = len * sizeof(sarray_temp);
	int totalCount = 0;
	int index;
	int oldBucketValue;
	sarray_temp *new_stemp = (sarray_temp*)malloc(array_size);

	memset(buckets, 0, 256 * sizeof(int));

	for (int i = 0; i < len; i++) {
		++buckets[stemp[i].first_block_order];
	}

	for (int i = 0; i < 256; i++) {
		oldBucketValue = buckets[i];
		buckets[i] = totalCount;
		totalCount += oldBucketValue;
	}

	for (int i = 0; i < len; i++) {
		index = stemp[i].first_block_order;
		new_stemp[buckets[index]].first_block_order = index;
		new_stemp[buckets[index]].second_block_order = stemp[i].second_block_order;
		new_stemp[buckets[index]].start_index = stemp[i].start_index;

		++buckets[index];
	}

	free(stemp);

	return new_stemp;
}

// Construindo o sarray
int* build_sarray(char* text, int text_length, int explimit) {
	int array_size = text_length * sizeof(int);
	int aux = 0;
	int group_size = 1;
	int stemp_arraysize = text_length * sizeof(sarray_temp);
	sarray_temp* stemp = (sarray_temp*)malloc(stemp_arraysize);
	int *final_sarray = (int*)malloc(array_size);

	for (int i = 0; i < text_length; ++i) {
		stemp[i].first_block_order = text[i];
		stemp[i].second_block_order = i;
		stemp[i].start_index = 0;
	}

	stemp = sort_letter_occurrences(stemp, text_length);

	for (int i = 0; i < text_length; ++i) {
		if (i > 0 && stemp[i].first_block_order != stemp[i - 1].first_block_order)
			++aux;

		final_sarray[stemp[i].second_block_order] = aux;
	}

	for (int k = 0; k < explimit; ++k) {
		for (int i = 0; i < text_length; ++i) {
			stemp[i].first_block_order = final_sarray[i];
			stemp[i].start_index = i;

			if (i + group_size < text_length) {
				stemp[i].second_block_order = final_sarray[i + group_size];
			} else {
				stemp[i].second_block_order = -1;
			}
		}

		std::sort(stemp, stemp + text_length);

		aux = 0;

		for (int i = 0; i < text_length; ++i) {
			if (i > 0
					&&
					(stemp[i].first_block_order != stemp[i - 1].first_block_order
					|| stemp[i].second_block_order != stemp[i - 1].second_block_order))
				++aux;

			final_sarray[stemp[i].start_index] = aux;
		}

		group_size <<= 1;
	}

	for (int i = 0; i < text_length; ++i) {
		final_sarray[i] = stemp[i].start_index;
	}

	free(stemp);

	return final_sarray;
}

int lcp(char* text1, char* text2) {
	int lcp = 0;

	while(text1[lcp] != '\0'
			&& text2[lcp] != '\0'
			&& text1[lcp] == text2[lcp]) {
		++lcp;
	}

	return lcp;
}

void build_LRlcp(int* Llcp, int* Rlcp, int* sarray, char* text, int left, int right) {
	int middle;

	if (right - left > 1) {
		middle = (left + right) / 2;

		Llcp[middle] = lcp(&text[sarray[left]], &text[sarray[middle]]);
		Llcp[middle] = lcp(&text[sarray[middle]], &text[sarray[right]]);

		build_LRlcp(Llcp, Rlcp, sarray, text, left, middle);
		build_LRlcp(Llcp, Rlcp, sarray, text, middle, right);
	}
}

// Os parâmetros sarray, Llcp e Rlcp devem ser passados por referência, devendo
// ser na verdade ponteiros para inteiros.
void build_sarray_LRlcp(char* text, int text_length, int** sarray, int** Llcp, int** Rlcp) {
	int explimit = ceil(log2(text_length));
	size_t array_size = text_length * sizeof(int);

	(*Llcp) = (int*)malloc(array_size);
	memset(*Llcp, -1, array_size);
	(*Rlcp) = (int*)malloc(array_size);
	memset(*Rlcp, -1, array_size);

	(*sarray) = build_sarray(text, text_length, explimit);

	build_LRlcp(*Llcp, *Rlcp, *sarray, text, 0, text_length - 1);
}

int predecessor(char* text, int txtlen, char* pattern, int patlen, int* sarray, int* Llcp, int* Rlcp) {
	int L = lcp(pattern, &text[sarray[0]]);
	int R = lcp(pattern, &text[sarray[txtlen-1]]);
	int l = 0;
	int r = txtlen - 1;
	int h, H;

	// se o padrão for maior ou igual que o último sufixo
	if (R == patlen
			|| R + sarray[txtlen - 1] == txtlen
			|| text[R + sarray[txtlen - 1]] < pattern[R]) {
		return txtlen - 1;
	}

	// se o padrão for menor que o primeiro sufixo
	if (L < patlen
			&& L + sarray[0] < txtlen
			&& text[L + sarray[0]] > pattern[L]) {
		return -1;
	}

	while (r - l > 1) {
		h = (l + r) / 2;

		if (L >= R) {
			if (L < Llcp[h]) {
				l = h;
			} else if (L == Llcp[h]) {
				H = L + lcp(&pattern[L], &text[sarray[h] + L]);

				// se o padrão for maior ou igual a o sufixo H
				if (H == patlen
						|| txtlen == H + sarray[h]
						|| text[sarray[h] + H] < pattern[H]) {
					l = h;
					L = H;
				} else {
					r = h;
					R = H;
				}
			} else {
				r = h;
				R = Llcp[h];
			}
		} else { // R > L
			if (R < Rlcp[h]) {
				r = h;
			} else if (R == Rlcp[h]) {
				H = R + lcp(&pattern[R], &text[sarray[h] + R]);

				// se o padrão for maior ou igual ao sufixo H
				if (H == patlen
						|| txtlen == H + sarray[h]
						|| text[sarray[h] + H] < pattern[H]) {
					l = h;
					L = H;
				} else {
					r = h;
					R = H;
				}
			} else {
				l = h;
				L = Rlcp[h];
			}
		}
	}

	return l;
}

int successor(char* text, int txtlen, char* pattern, int patlen, int* sarray, int* Llcp, int* Rlcp) {
	int L = lcp(pattern, &text[sarray[0]]);
	int R = lcp(pattern, &text[sarray[txtlen - 1]]);
	int l = 0;
	int r = txtlen - 1;
	int h, H;

	// se o padrão for menor ou igual ao primeiro sufixo
	if (L == patlen
			||
		(sarray[0] + L < txtlen
			&& pattern[L] < text[sarray[0] + L])) {
		return 0;
	}

	// se o padrão for maior que o último sufixo
	if ((sarray[txtlen - 1] + R == txtlen
		&& R < patlen)
			||
		(R < patlen
			&& sarray[txtlen - 1] + R < txtlen
			&& pattern[R] > text[sarray[txtlen - 1] + R])) {
		return txtlen;
	}

	while (r - l > 1) {
		h = (l + r) / 2;

		if (L >= R) {
			if (L < Llcp[h]) {
				l = h;
			} else if (L == Llcp[h]) {
				H = L + lcp(&pattern[L], &text[sarray[h] + L]);

				// se o padrão for menor ou igual ao sufixo H
				if (H == patlen
							||
						(H + sarray[h] < txtlen
						&& text[sarray[h] + H] > pattern[H])) {
					r = h;
					R = H;
				} else {
					l = h;
					L = H;
				}
			} else {
				r = h;
				R = Llcp[h];
			}
		} else { // R > L
			if (R < Rlcp[h]) {
				r = h;
			} else if (R == Rlcp[h]) {
				H = R + lcp(&pattern[R], &text[sarray[h] + R]);

				// se o padrão for menor ou igual ao sufixo H
				if (H == patlen
						||
						(H + sarray[h] < txtlen
						&& text[sarray[h] + H] > pattern[H])) {
					r = h;
					R = H;
				} else {
					l = h;
					L = H;
				}
			} else {
				l = h;
				L = Rlcp[h];
			}
		}
	}

	return r;
}

void find_occurrences(int* matches_start, int* matches_end, char* text, int txtlen, char* pattern, int patlen, int* sarray, int* Llcp, int* Rlcp) {
	int pred = predecessor(text, txtlen, pattern, patlen, sarray, Llcp, Rlcp);
	int succ = successor(text, txtlen, pattern, patlen, sarray, Llcp, Rlcp);

	(*matches_start) = succ;
	(*matches_end) = pred;
}

// A ser usado para a compressão
char* get_bytes_from_array(int* array, uint32_t arraylen) {
	uint32_t result_size = arraylen * sizeof(int);
	char *result = (char*)malloc(result_size);
	uint32_t result_index = 0;
	uint32_t array_index = 0;

	while(array_index < arraylen) {
		result[result_index++] = (array[array_index] & 0xff000000) >> 24;
		result[result_index++] = (array[array_index] & 0x00ff0000) >> 16;
		result[result_index++] = (array[array_index] & 0x0000ff00) >> 8;
		result[result_index++] = array[array_index] & 0x000000ff;

		++array_index;
	}

	return result;
}

int* get_int_array_from_bytes(char* byte_array, uint32_t arraylen) {
	uint32_t result_size = arraylen * sizeof(int);
	int* result = (int*)malloc(result_size);
	int aux;

	uint32_t result_index = 0;
	uint32_t bytearray_index = 0;

	while (result_index < result_size) {
		aux = 0 | byte_array[bytearray_index] << 24
			| byte_array[bytearray_index + 1] << 16
			| byte_array[bytearray_index + 2] << 8
			| byte_array[bytearray_index + 3];

		result[result_index++] = aux;
		bytearray_index += 4;
	}

	return result;
}
