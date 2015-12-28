#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "sarray.h"

typedef struct sarray_temp {
	int first_block_order;
	int second_block_order;
	int start_index;
} sarray_temp;

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

int cmp(sarray_temp* stemp, int index1, int index2) {
	if (stemp[index1].first_block_order == stemp[index2].first_block_order) {
		if (stemp[index1].second_block_order == stemp[index2].second_block_order) {
			return stemp[index1].start_index - stemp[index2].start_index;
		}
		return stemp[index1].second_block_order - stemp[index2].second_block_order;
	}
	return stemp[index1].first_block_order - stemp[index2].first_block_order;
}

void heapify_temp_sarray(int index, int length, sarray_temp* stemp) {
	int childIndex = (index * 2) + 1;
	sarray_temp swap_aux;

	while (childIndex < length) {
		if ((length - childIndex > 1)
					&& cmp(stemp, childIndex, childIndex + 1) < 0) {
			++childIndex;
		}

		if (cmp(stemp, index, childIndex) > 0) break;

		swap_aux = stemp[index];
		stemp[index] = stemp[childIndex];
		stemp[childIndex] = swap_aux;

		index = childIndex;
		childIndex = (index * 2) + 1;
	}
}

void heapsort_temp_sarray(sarray_temp* stemp, int length) {
	int i;
	sarray_temp swap_aux;

	for (i = (length - 1) / 2; i >= 0; --i) {
		heapify_temp_sarray(i, length, stemp);
	}

	for (i = length - 1; i >= 1; --i) {
		swap_aux = stemp[i];
		stemp[i] = stemp[0];
		stemp[0] = swap_aux;

		heapify_temp_sarray(0, i, stemp);
	}
}

int* build_sarray(char* text, int text_length, sarray_temp** P, int explimit) {
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
		P[k] = (sarray_temp*)malloc(stemp_arraysize);

		for (int i = 0; i < text_length; ++i) {
			P[k][i].first_block_order = final_sarray[i];
			stemp[i].first_block_order = final_sarray[i];
			P[k][i].start_index = i;
			stemp[i].start_index = i;

			if (i + group_size < text_length) {
				P[k][i].second_block_order = final_sarray[i + group_size];
				stemp[i].second_block_order = final_sarray[i + group_size];
			} else {
				P[k][i].second_block_order = -1;
				stemp[i].second_block_order = -1;
			}
		}

		heapsort_temp_sarray(stemp, text_length);

		aux = 0;

		for (int i = 0; i < text_length; ++i) {
			if (i > 0 &&
					(stemp[i].first_block_order != stemp[i - 1].first_block_order
						|| stemp[i].second_block_order != stemp[i - 1].second_block_order))
				++aux;

			final_sarray[stemp[i].start_index] = aux;
		}

		group_size <<= 1;
	}

	free(stemp);

	return final_sarray;
}

int compute_lcp(sarray_temp** P, int i, int j, int text_length, int explimit) {
	int lcp = 0;
	int k = explimit - 1;
	int aux;

	if (i == j) {
		return text_length - i;
	}

	while (k >= 0 && i < text_length && j < text_length) {
		if (P[k][i].first_block_order == P[k][j].first_block_order) {
			aux = 1 << k;
			lcp += aux;
			i += aux;
			j += aux;
		}

		--k;
	}

	return lcp;
}

void build_LRlcp(int* Llcp, int* Rlcp, int* sarray, sarray_temp** P, int explimit, int text_length, int left, int right) {
	int middle;

	if (right - left > 1) {
		middle = (left + right) / 2;

		Llcp[middle] = compute_lcp(P, sarray[left], sarray[middle], text_length, explimit);
		Rlcp[middle] = compute_lcp(P, sarray[middle], sarray[right], text_length, explimit);

		build_LRlcp(Llcp, Rlcp, sarray, P, explimit, text_length, left, middle);
		build_LRlcp(Llcp, Rlcp, sarray, P, explimit, text_length, middle, right);
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

	// estrutura auxiliar que guarda todos os p^k intermediários
	// para a construção dos L/Rlcp
	sarray_temp** P = (sarray_temp**)malloc(explimit * sizeof(sarray_temp*));

	(*sarray) = build_sarray(text, text_length, P, explimit);

	build_LRlcp(*Llcp, *Rlcp, *sarray, P, explimit, text_length, 0, text_length - 1);

	for (int i = 0; i < explimit; i++) {
		free(P[i]);
	}
	free(P);
}
