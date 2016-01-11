#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <algorithm>

#include "sarray.h"

typedef struct sarray_temp {
	long first_block_order;
	long second_block_order;
	size_t start_index;
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
sarray_temp* sort_letter_occurrences(sarray_temp* stemp, size_t len) {
	long buckets[256];
	size_t array_size = len * sizeof(sarray_temp);
	size_t totalCount = 0;
	long index;
	size_t oldBucketValue;
	sarray_temp *new_stemp = (sarray_temp*)malloc(array_size);

	memset(buckets, 0, 256 * sizeof(size_t));

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
size_t* build_sarray(char* text, size_t text_length, uint32_t explimit, size_t *sarray_inverse) {
	size_t array_size = text_length * sizeof(size_t);
	long aux = 0;
	size_t group_size = 1;
	size_t stemp_arraysize = text_length * sizeof(sarray_temp);
	sarray_temp* stemp = (sarray_temp*)malloc(stemp_arraysize);
	size_t *final_sarray = (size_t*)malloc(array_size);

	for (size_t i = 0; i < text_length; ++i) {
		stemp[i].first_block_order = text[i];
		stemp[i].second_block_order = i;
		stemp[i].start_index = 0;
	}

	stemp = sort_letter_occurrences(stemp, text_length);

	for (size_t i = 0; i < text_length; ++i) {
		if (i > 0 && stemp[i].first_block_order != stemp[i - 1].first_block_order)
			++aux;

		final_sarray[stemp[i].second_block_order] = aux;
	}

	for (uint32_t k = 0; k < explimit; ++k) {
		for (size_t i = 0; i < text_length; ++i) {
			stemp[i].first_block_order = final_sarray[i];
			stemp[i].start_index = i;

			if (i + group_size < text_length) {
				stemp[i].second_block_order = final_sarray[i + group_size];
			} else {
				stemp[i].second_block_order = -1;
			}
		}

		std::sort(stemp, &stemp[text_length]);

		aux = 0;

		for (size_t i = 0; i < text_length; ++i) {
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
		sarray_inverse[stemp[i].start_index] = i;
	}

	free(stemp);

	return final_sarray;
}

size_t lcp(char* text1, char* text2) {
	size_t lcp = 0;

	while(text1[lcp] != '\0'
			&& text2[lcp] != '\0'
			&& text1[lcp] == text2[lcp]) {
		++lcp;
	}

	return lcp;
}

size_t build_LRlcp(size_t* Llcp, size_t* Rlcp, size_t* lcp_array, size_t left, size_t right) {
	size_t middle;
	size_t lcpleft;
	size_t lcpright;

	if (right - left > 1) {
		middle = (left + right) / 2;

		lcpleft = build_LRlcp(Llcp, Rlcp, lcp_array, left, middle);
		lcpright = build_LRlcp(Llcp, Rlcp, lcp_array, middle, right);

		Llcp[middle] = lcpleft;
		Rlcp[middle] = lcpright;

		if (lcpleft < lcpright) {
			return lcpleft;
		} else {
			return lcpright;
		}
	} else {
		return lcp_array[right];
	}
}

size_t *build_lcp_array(char *text, size_t *sarray, size_t *sarray_inverse, size_t size) {
	size_t *result = (size_t*)calloc(size, sizeof(size_t));
	size_t last_lcp = 0;
	size_t k, j;

	for (size_t i = 0; i < size; i++) {
		k = sarray_inverse[i];

		if (k) {
			j = sarray[k - 1];

			while (text[i + last_lcp] && text[j + last_lcp]
					&& text[i + last_lcp] == text[j + last_lcp]) {
				++last_lcp;
			}

			result[k] = last_lcp;
		}

		if (last_lcp > 0) {
			--last_lcp;
		}
	}

	return result;
}

// Os parâmetros sarray, Llcp e Rlcp devem ser passados por referência, devendo
// ser na verdade ponteiros para inteiros.
void build_sarray_LRlcp(char* text, size_t text_length, size_t** sarray, size_t** Llcp, size_t** Rlcp) {
	uint32_t explimit = ceil(log2(text_length));
	size_t array_size = text_length * sizeof(size_t);
	size_t *sarray_inverse = (size_t*)calloc(text_length, sizeof(size_t));

	(*sarray) = build_sarray(text, text_length, explimit, sarray_inverse);

	size_t *lcp_array = build_lcp_array(text, *sarray, sarray_inverse, text_length);

	free(sarray_inverse);

	(*Llcp) = (size_t*)malloc(array_size);
	memset(*Llcp, -1, array_size);
	(*Rlcp) = (size_t*)malloc(array_size);
	memset(*Rlcp, -1, array_size);

	build_LRlcp(*Llcp, *Rlcp, lcp_array, 0, text_length - 1);

	free(lcp_array);
}

size_t predecessor(char* text, size_t txtlen, char* pattern, size_t patlen, size_t* sarray, size_t* Llcp, size_t* Rlcp) {
	size_t L = lcp(pattern, &text[sarray[0]]);
	size_t R = lcp(pattern, &text[sarray[txtlen-1]]);
	size_t l = 0;
	size_t r = txtlen - 1;
	size_t h, H;

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

size_t successor(char* text, size_t txtlen, char* pattern, size_t patlen, size_t* sarray, size_t* Llcp, size_t* Rlcp) {
	size_t L = lcp(pattern, &text[sarray[0]]);
	size_t R = lcp(pattern, &text[sarray[txtlen - 1]]);
	size_t l = 0;
	size_t r = txtlen - 1;
	size_t h, H;

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

void find_occurrences(size_t* matches_start, size_t* matches_end, char* text, size_t txtlen, char* pattern, size_t patlen, size_t* sarray, size_t* Llcp, size_t* Rlcp) {
	size_t pred = predecessor(text, txtlen, pattern, patlen, sarray, Llcp, Rlcp);
	size_t succ = successor(text, txtlen, pattern, patlen, sarray, Llcp, Rlcp);

	(*matches_start) = succ;
	(*matches_end) = pred;
}

// A ser usado para a compressão
char* get_bytes_from_array(size_t* array, size_t arraylen) {
    size_t result_size = arraylen << 3;
    char *result = (char*)malloc(result_size);
    size_t result_index = 0;
    size_t array_index = 0;

    while(array_index < arraylen) {
        result[result_index++] = (array[array_index] & 0xff00000000000000) >> 56;
        result[result_index++] = (array[array_index] & 0xff000000000000) >> 48;
        result[result_index++] = (array[array_index] & 0xff0000000000) >> 40;
        result[result_index++] = (array[array_index] & 0xff00000000) >> 32;
        result[result_index++] = (array[array_index] & 0xff000000) >> 24;
        result[result_index++] = (array[array_index] & 0xff0000) >> 16;
        result[result_index++] = (array[array_index] & 0xff00) >> 8;
        result[result_index++] = (array[array_index] & 0xff);

        ++array_index;
    }

    return result;
}

size_t* get_int_array_from_bytes(char* byte_array, size_t arraylen) {
	size_t result_size = arraylen << 3;
	size_t* result = (size_t*)malloc(result_size);
	size_t aux;

	size_t result_index = 0;
	size_t bytearray_index = 0;

	while (bytearray_index < result_size) {
		aux = 0;
		aux |= ((size_t)byte_array[bytearray_index++] & 0xff) << 56;
		aux |= ((size_t)byte_array[bytearray_index++] & 0xff) << 48;
		aux |= ((size_t)byte_array[bytearray_index++] & 0xff) << 40;
		aux |= ((size_t)byte_array[bytearray_index++] & 0xff) << 32;
		aux |= ((size_t)byte_array[bytearray_index++] & 0xff) << 24;
		aux |= ((size_t)byte_array[bytearray_index++] & 0xff) << 16;
		aux |= ((size_t)byte_array[bytearray_index++] & 0xff) << 8;
		aux |= ((size_t)byte_array[bytearray_index++] & 0xff);

		result[result_index++] = aux;
	}

	return result;
}
