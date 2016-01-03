#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>

void encode_pointer(uint16_t jump, uint8_t length, uint8_t *code_ptr1, uint8_t *code_ptr2) {
	(*code_ptr1) = ((0x0ff0 & jump) >> 4);
	(*code_ptr2) = ((0x000f & jump) << 4) | (length & 0x000f);
}

void decode_pointer(uint8_t code_ptr1, uint8_t code_ptr2, uint16_t *jump, uint8_t *length) {
	(*jump) = ((code_ptr1 << 4) | (code_ptr2 >> 4));
	(*length) = code_ptr2 & 0x000f;
}

void print_array(uint8_t *array, int length) {
	uint16_t jump;
	uint8_t match_len;

	printf("\n");
	for (int i = 0; i < length;) {
		decode_pointer(array[i++], array[i++], &jump, &match_len);
		printf("%u %u '%c'\n", jump, match_len, array[i++]);
	}
	printf("\n");
}

void prefix_match(char* text, int txtlen, int cursor, int Ls, int Ll, uint16_t* jump, uint8_t* length) {
	uint8_t max_matches = 0;
	uint16_t result_jump = 0;
	int i = 0;
	uint8_t matches;
	int window_limit = cursor + Ll - 1;

	if (txtlen < window_limit) {
		window_limit = txtlen;
	}

	if (cursor > Ls) {
		i = cursor - Ls;
	}

	while (i < cursor) {
		matches = 0;

		while (cursor + matches <= window_limit
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

uint8_t* lz77_encode(char* txt, int txtlen, int Ls, int Ll, int* code_length) {
	int cursor = 0;
	int code_index = 0;
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

void lz77_decode(uint8_t *code, int codelen, int Ls, int Ll, char* txt) {
	uint16_t jump;
	uint8_t match_length;
	int i = 0;
	int j;
	int text_index = 0;

	while (i < codelen) {
		decode_pointer(code[i++], code[i++], &jump, &match_length);

		j = text_index - jump;
		while (match_length--) {
			txt[text_index++] = txt[j++];
		}
		txt[text_index++] = code[i++];
	}
	if (txt[text_index] != '\0')
		txt[++text_index] = '\0';
}

int main() {
	char* txt = "carai";
	int txtlen = strlen(txt);
	char* decoded_text = (char*)malloc((txtlen+1)*sizeof(char));
	int search_window_length = 1 << 12;
	int lookahead_length = 1 << 4;
	int codelen = 0;

	uint8_t* encoded_text = lz77_encode(txt, txtlen, search_window_length, lookahead_length, &codelen);

	print_array(encoded_text, codelen);

	lz77_decode(encoded_text, codelen, search_window_length, lookahead_length, decoded_text);

	printf("decoded text: %s\n", decoded_text);
	printf("text length: %d, resulting length: %d\n", txtlen, (codelen * 3));
	printf("strcmp: %d\n", strcmp(txt, decoded_text));
}
