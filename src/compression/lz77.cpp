#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "lz77.h"

static inline void encode_pointer(uint16_t jump, uint8_t length, uint8_t *code_ptr1, uint8_t *code_ptr2) {
	(*code_ptr1) = ((0x0ff0 & jump) >> 4);
	(*code_ptr2) = ((0x000f & jump) << 4) | (length & 0x000f);
}

static inline void decode_pointer(uint8_t code_ptr1, uint8_t code_ptr2, uint16_t *jump, uint8_t *length) {
	(*jump) = ((code_ptr1 << 4) | (code_ptr2 >> 4));
	(*length) = code_ptr2 & 0x000f;
}

void prefix_match(char* text, size_t txtlen, size_t cursor, int Ls, int Ll, uint16_t* jump, uint8_t* length) {
	uint8_t max_matches = 0;
	uint16_t result_jump = 0;
	size_t i = 0;
	uint8_t matches;
	size_t window_limit = cursor + Ll - 1;

	if (txtlen < window_limit) {
		window_limit = txtlen - 1;
	}

	if (cursor > Ls) {
		i = cursor - Ls;
	}

	while (i < cursor) {
		matches = 0;

		while (cursor + matches < window_limit
				&& text[i + matches] == text[cursor + matches]) {
			++matches;
		}

		if (matches > max_matches) {
			max_matches = matches;
			result_jump = cursor - i;
		}

		++i;
	}

	(*length) = max_matches;
	(*jump) = result_jump;
}

uint8_t* lz77_encode(char* txt, size_t txtlen, int Ls, int Ll, size_t* code_length) {
	size_t cursor = 0;
	size_t code_index = 0;
	uint8_t* code = (uint8_t*)malloc(3 * txtlen * sizeof(uint8_t));
	uint16_t jump;
	uint8_t match_length, next_char, ptr1, ptr2;

	while (cursor < txtlen) {
		prefix_match(txt, txtlen, cursor, Ls, Ll, &jump, &match_length);

		encode_pointer(jump, match_length, &ptr1, &ptr2);
		next_char = txt[cursor + match_length];

		code[code_index++] = ptr1;
		code[code_index++] = ptr2;
		code[code_index++] = next_char;

		cursor += (match_length + 1);
	}

	(*code_length) = code_index;

	return code;
}

void lz77_decode(uint8_t *code, size_t codelen, int Ls, int Ll, char* txt) {
	uint16_t jump;
	uint8_t match_length;
	size_t i = 0;
	size_t j;
	size_t text_index = 0;

	while (i < codelen) {
		decode_pointer(code[i], code[i + 1], &jump, &match_length);

		j = text_index - jump;
		while (match_length--) {
			txt[text_index++] = txt[j++];
		}
		txt[text_index++] = code[i + 2];

		i += 3;
	}
}
