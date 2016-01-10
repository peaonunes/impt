#include <getopt.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <cstdint>
#include <sys/types.h>
#include <sys/stat.h>

#include "utils.h"
#include "input/FileReader.h"
#include "sarray/sarray.h"
#include "compression/lz78.h"
#include "compression/lz77.h"

using namespace std;

const int PRINT_OCC_SOFT_LIMIT = 10;
const int PRINT_OCC_HARD_LIMIT = 20;

program_args::program_args()
: mode_flag(0),
compression_flag(LZ78),
pattern_file(0),
help_flag(false),
count_flag(false),
index_file(0),
text_file(0),
largest_pattern_length(0) { }

program_args::~program_args() { }

program_args get_program_parameters(int argc, char** argv) {

	int option_index;
	int current_parameter;
	program_args args;

	struct option long_options[] =
	{
		{"pattern", 	required_argument, 0, 'p'},
		{"help",    	no_argument,       0, 'h'},
		{"count",   	no_argument,       0, 'c'},
		{"compression", required_argument, 0, 'x'},
		{0, 0, 0, 0}
	};

	if (argc > 1) {
		char* mode = argv[1];
		if (strcmp(mode, "index") == 0) {
			args.mode_flag = INDEX_MODE;
			optind++;
		} else if (strcmp(mode, "search") == 0) {
			args.mode_flag = SEARCH_MODE;
			optind++;
		}
	}

	while (1) {
		current_parameter = getopt_long(argc, argv, "p:hcx:", long_options, &option_index);

		if (current_parameter == -1) {
			break;
		}

		switch (current_parameter) {
			case 0:
			// No momento, nenhum argumento está sendo usado para setar uma flag
			break;
			case 'p':
			args.pattern_file = optarg;
			break;
			case 'h':
			args.help_flag = true;
			break;
			case 'c':
			args.count_flag = true;
			break;
			case 'x':
			if (strcmp(optarg, "LZ77") == 0) {
				args.compression_flag = LZ77;
			} else if (strcmp(optarg, "LZ78") == 0) {
				args.compression_flag = LZ78;
			} else {
				cerr << "Algoritmo de compressão inválido.\n"; exit(1);
			}
			break;
			case '?':
			// Um argumento desconhecido é apenas ignorado no momento
			break;
			default:
			exit(1);
		}
	}

	if (args.mode_flag == INDEX_MODE) {
		if (optind < argc) {
			args.text_file = argv[optind++];
		}
	} else if (args.mode_flag == SEARCH_MODE) {
		if (optind < argc) {
			if (!args.pattern_file) {
				args.largest_pattern_length = strlen(argv[optind]);
				args.patterns.push_back(argv[optind++]);
			}
		}
		if (optind < argc) {
			args.index_file = argv[optind++];
		}
	}

	return args;
}

void print_help_line(char const *msg1, char const *msg2) {
	cout << left << setw(35) << msg1 << setw(45) << msg2 << endl;
}

void print_help_text() {
	cout << "Usage:" << endl;
	cout << "Index mode: ipmt index textfile" << endl;
	cout << "Options:" << endl;
	cout << "  None" << endl;
	cout << "Search mode: ipmt search pattern indexfile" << endl;
	cout << "Options:" << endl;
	print_help_line("  -c, --count", "Counts the pattern occurrences in the text");
	print_help_line("  -p, --pattern=<pattern file>","Specifies file from which the program should read the patterns to be used (each line of the file specifies a pattern)");
	print_help_line("  -h, --help","Shows this message");
	cout << endl << "  If a pattern file is not specified, the first argument given to pmt will be read as the only pattern to be searched for in the text file. Several source text files can be specified at the same time." << endl;
}

int is_regular_file(const char *path) {
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

void read_pattern_file(program_args &args) {
	FileReader fr(args.pattern_file);
	string buffer;

	while(fr.hasContent()) {
		buffer = fr.readLine();

		if (buffer.size()) {
			if (buffer.size() > args.largest_pattern_length) {
				args.largest_pattern_length = buffer.size();
			}

			args.patterns.push_back(buffer);
		}
	}
}

void search_index_file(program_args &args) {
	// decompress
	FILE* fp = fopen(args.index_file, "r");
	size_t size, byte_array_size;
	size_t code_len;
	char* text;
	uint8_t* encoded_byte_array;
	int Ls = (1 << 12) - 1;
	int Ll = (1 << 4) - 1;
	size_t* sarray;
	size_t* Llcp;
	size_t* Rlcp;
	size_t* sorted_occurrences;
	size_t start, end, start_print, end_print;
	size_t starting_point, pattern_size, total_occ;
	char *byte_array, *print_occ;
	uint8_t compression_mode;

	fread(&compression_mode, sizeof(uint8_t), 1, fp);
	fread(&size, sizeof(size_t), 1, fp);
	byte_array_size = size * sizeof(size_t);

	// Text
	fread(&code_len, sizeof(size_t), 1, fp);
	encoded_byte_array = (uint8_t*)malloc(code_len * sizeof(uint8_t));
	fread(encoded_byte_array, sizeof(uint8_t), code_len, fp);

	if (compression_mode == LZ77) {
		text = (char*)malloc((size + 1) * sizeof(char));
		lz77_decode(encoded_byte_array, code_len, Ls, Ll, text);
		text[size] = 0;
	} else if (compression_mode == LZ78) {
		text = lz78_decode(encoded_byte_array, code_len, size);
	}
	free(encoded_byte_array);

	// Suffix array
	fread(&code_len, sizeof(size_t), 1, fp);
	encoded_byte_array = (uint8_t*)malloc(code_len * sizeof(uint8_t));
	fread(encoded_byte_array, sizeof(uint8_t), code_len, fp);

	if (compression_mode == LZ77) {
		byte_array = (char*)malloc((byte_array_size) * sizeof(char));
		lz77_decode(encoded_byte_array, code_len, Ls, Ll, byte_array);
	} else if (compression_mode == LZ78) {
		byte_array = lz78_decode(encoded_byte_array, code_len, byte_array_size);
	}

	free(encoded_byte_array);
	sarray = get_int_array_from_bytes(byte_array, size);
	free(byte_array);

	// Llcp
	fread(&code_len, sizeof(size_t), 1, fp);
	encoded_byte_array = (uint8_t*)malloc(code_len * sizeof(uint8_t));
	fread(encoded_byte_array, sizeof(uint8_t), code_len, fp);

	if (compression_mode == LZ77) {
		byte_array = (char*)malloc((byte_array_size) * sizeof(char));
		lz77_decode(encoded_byte_array, code_len, Ls, Ll, byte_array);
	} else if (compression_mode == LZ78) {
		byte_array = lz78_decode(encoded_byte_array, code_len, byte_array_size);
	}

	free(encoded_byte_array);
	Llcp = get_int_array_from_bytes(byte_array, size);
	free(byte_array);

	// Rlcp
	fread(&code_len, sizeof(size_t), 1, fp);
	encoded_byte_array = (uint8_t*)malloc(code_len * sizeof(uint8_t));
	fread(encoded_byte_array, sizeof(uint8_t), code_len, fp);

	if (compression_mode == LZ77) {
		byte_array = (char*)malloc((byte_array_size) * sizeof(char));
		lz77_decode(encoded_byte_array, code_len, Ls, Ll, byte_array);
	} else if (compression_mode == LZ78) {
		byte_array = lz78_decode(encoded_byte_array, code_len, byte_array_size);
	}

	free(encoded_byte_array);
	Rlcp = get_int_array_from_bytes(byte_array, size);
	free(byte_array);

	fclose(fp);

	// Loop through patterns and search
	sorted_occurrences = (size_t*)malloc(size * sizeof(size_t));
	byte_array = (char*)malloc(args.largest_pattern_length + 1);
	print_occ = (char*)malloc((PRINT_OCC_HARD_LIMIT * 2) + args.largest_pattern_length + 1);
	for (int i = 0; i < args.patterns.size(); ++i) {
		strcpy(byte_array, args.patterns[i].c_str());
		pattern_size = strlen(byte_array);

		find_occurrences(&start, &end, text, size, byte_array, pattern_size, sarray, Llcp, Rlcp);
		total_occ = end - start + 1;

		printf("Padrão: %s\n", byte_array);
		if (!total_occ) {
			printf("Nenhuma ocorrência encontrada.\n");
		} else if (total_occ == 1) {
			printf("1 ocorrência:\n");
		} else {
			printf("%lu ocorrências\n", total_occ);
		}
		if (!args.count_flag && total_occ) {
			memcpy(sorted_occurrences, &sarray[start], total_occ * sizeof(size_t));
			std::sort(sorted_occurrences, &sorted_occurrences[total_occ - 1]);

			for (int j = 0; j < total_occ; ++j) {
				start_print = starting_point = sorted_occurrences[j];
				end_print = start_print + pattern_size;

				while (start_print > 0
					&& starting_point - start_print < PRINT_OCC_HARD_LIMIT
					&& text[start_print - 1] != '\n'
					&& (starting_point - start_print < PRINT_OCC_SOFT_LIMIT
						|| text[start_print - 1] != ' ')) {
					--start_print;
				}

				starting_point = end_print;
				while (end_print < size
					&& end_print - starting_point < PRINT_OCC_HARD_LIMIT
					&& text[end_print + 1] != '\n'
					&& (end_print - starting_point < PRINT_OCC_SOFT_LIMIT
						|| text[end_print + 1] != ' ')) {
					++end_print;
				}

				strncpy(print_occ, (text + start_print), (end_print - start_print + 1));
				print_occ[end_print - start_print + 1] = 0;

				if (start_print) printf("[...]");
				printf("%s", print_occ);
				if (text[end_print]) printf("[...]");
				printf("\n");
			}
		}
	}
	free(byte_array);
	free(print_occ);
	free(sorted_occurrences);
	free(text);
	free(sarray);
	free(Llcp);
	free(Rlcp);
}

void print_sarray(size_t* sarray, char* text, size_t size) {
	for (size_t i = 0; i < size; i++) {
		printf("%lu %s\n", sarray[i], &text[sarray[i]]);
	}
}

void create_index_file(program_args &args) {
	FILE* fp = fopen(args.text_file, "r");
	size_t size, byte_array_size;
	size_t code_len;
	char *text;

	/* index file name */
	char* after_last_slash_occurrence = strrchr(args.text_file, '/');
	if (!after_last_slash_occurrence)
		after_last_slash_occurrence = args.text_file;
	else
		after_last_slash_occurrence = after_last_slash_occurrence + 1;
	char* index_name = (char*)malloc(strlen(after_last_slash_occurrence) + 5);
	strcpy(index_name, after_last_slash_occurrence);
	strcat(index_name, ".idx");

	size_t* sarray;
	size_t* Llcp;
	size_t* Rlcp;
	char* byte_array;
	uint8_t* encoded_byte_array;
	int Ls = (1 << 12) - 1;
	int Ll = (1 << 4) - 1;

	if (fp) {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		byte_array_size = size << 3;
		fseek(fp, 0, SEEK_SET);
		text = (char*)malloc((size + 1)*sizeof(char));
		fread(text, sizeof(char), size, fp);
		text[size] = 0;
		fclose(fp);

		build_sarray_LRlcp(text, size, &sarray, &Llcp, &Rlcp);

		fp = fopen(index_name, "wb+");
		free(index_name);

		if (fp) {
			fwrite(&args.compression_flag, sizeof(uint8_t), 1, fp);

			if (args.compression_flag == LZ77)
				encoded_byte_array = lz77_encode(text, size, Ls, Ll, &code_len);
			else if (args.compression_flag == LZ78)
				encoded_byte_array = lz78_encode(text, size, &code_len);
			fwrite(&size, sizeof(size_t), 1, fp);
			fwrite(&code_len, sizeof(size_t), 1, fp);
			fwrite(encoded_byte_array, sizeof(char), code_len, fp);
			free(encoded_byte_array);

			byte_array = get_bytes_from_array(sarray, size);
			free(sarray);
			if (args.compression_flag == LZ77)
				encoded_byte_array = lz77_encode(byte_array, byte_array_size, Ls, Ll, &code_len);
			else if (args.compression_flag == LZ78)
				encoded_byte_array = lz78_encode(byte_array, byte_array_size, &code_len);
			free(byte_array);
			fwrite(&code_len, sizeof(size_t), 1, fp);
			fwrite(encoded_byte_array, sizeof(char), code_len, fp);
			free(encoded_byte_array);

			byte_array = get_bytes_from_array(Llcp, size);
			free(Llcp);
			if (args.compression_flag == LZ77)
				encoded_byte_array = lz77_encode(byte_array, byte_array_size, Ls, Ll, &code_len);
			else if (args.compression_flag == LZ78)
				encoded_byte_array = lz78_encode(byte_array, byte_array_size, &code_len);
			free(byte_array);
			fwrite(&code_len, sizeof(size_t), 1, fp);
			fwrite(encoded_byte_array, sizeof(char), code_len, fp);
			free(encoded_byte_array);

			byte_array = get_bytes_from_array(Rlcp, size);
			free(Rlcp);
			if (args.compression_flag == LZ77)
				encoded_byte_array = lz77_encode(byte_array, byte_array_size, Ls, Ll, &code_len);
			else if (args.compression_flag == LZ78)
				encoded_byte_array = lz78_encode(byte_array, byte_array_size, &code_len);
			free(byte_array);
			fwrite(&code_len, sizeof(size_t), 1, fp);
			fwrite(encoded_byte_array, sizeof(uint8_t), code_len, fp);
			free(encoded_byte_array);

			fclose(fp);
		} else {
			printf("Erro ao abrir o arquivo %s\n", index_name);
			exit(1);
		}

	} else {
		printf("Erro ao abrir o arquivo %s\n", args.text_file);
		exit(1);
	}
}
