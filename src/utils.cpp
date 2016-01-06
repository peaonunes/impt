#include <getopt.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <glob.h>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <cstdint>

#include "utils.h"
#include "compression/lz77.h"
#include "sarray/sarray.h"

using namespace std;

program_args::program_args()
: pattern_file(0),
quiet_flag(false),
help_flag(false),
count_mode(false),
source_index_file(0) { }

program_args::~program_args() {

}

program_args get_program_parameters(int argc, char** argv) {
	int option_index;
	int current_parameter;
	program_args args;

	struct option long_options[] =
	{
		{"quiet",   no_argument,	   0, 'q'},
		{"count",   no_argument,	   0, 'c'},
		{"pattern", required_argument, 0, 'p'},
		{"help",	no_argument,	   0, 'h'},
		{0, 0, 0, 0}
	};

	while (1) {
		current_parameter = getopt_long(argc, argv, "qcp:h", long_options, &option_index);

		if (current_parameter == -1) {
			break;
		}

		switch (current_parameter) {
			case 0:
			// No momento, nenhum argumento está sendo usado para setar uma flag
			break;
			case 'q':
			args.quiet_flag = true;
			break;
			case 'c':
			args.count_mode = true;
			break;
			case 'p':
			args.pattern_file = optarg;
			break;
			case 'h':
			args.help_flag = true;
			break;
			case '?':
			// Um argumento desconhecido é apenas ignorado no momento
			break;
			default:
			exit(1);
		}
	}

	if (optind < argc) {
		/* Legacy text
		If pattern_flag is set, then the remaining arguments
		are target textfiles. Otherwise, the first one will be
		the pattern string*/
		if (!args.pattern_file) {
			args.patterns.push_back(argv[optind++]);
		}

		if (optind < argc) {
			args.source_index_file = argv[optind];
		}
	}

	return args;
}

void print_help_line(char const *msg1, char const *msg2) {
	cout << left << setw(35) << msg1 << setw(45) << msg2 << endl;
}

void print_help_text() {
	cout << "Usage: pmt [options] [pattern] textfilepath [textfilepath ...]" << endl;
	cout << "Options:" << endl;
	print_help_line("  -q, --quiet", "Inhibits every output message");
	print_help_line("  -c, --count", "Counts occurrences instead of showing them");
	print_help_line("  -p, --pattern=<pattern file>","Specifies file from which the program should read the patterns to be used (each line of the file specifies a pattern)");
	print_help_line("  -h, --help","Shows this message");
	cout << endl << "  If a pattern file is not specified, the first argument given to pmt will be read as the only pattern to be searched for in the text file. Several source text files can be specified at the same time." << endl;
}

void read_pattern_file(program_args &args) {
	// FileReader fr(args.pattern_file);
	// string buffer;
	//
	// while(fr.hasContent()) {
	// 	buffer = fr.readLine();
	//
	// 	if (buffer.size()) {
	// 		args.patterns.push_back(buffer);
	// 	}
	// }
}

void create_index_file(char* source_file) {
	FILE* fp = fopen(source_file, "r");
	uint32_t size;
	uint32_t code_len;
	char *text;
	char *index_name = "teste.idx";
	int* sarray;
	int* Llcp;
	int* Rlcp;
	char* byte_array;
	uint8_t* encoded_byte_array;
	int Ls = (1 << 12) - 1;
	int Ll = (1 << 4) - 1;

	if (fp) {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		text = (char*)malloc((size + 1)*sizeof(char));
		fread(text, sizeof(char), size, fp);
		text[size] = 0;
		fclose(fp);

		build_sarray_LRlcp(text, size, &sarray, &Llcp, &Rlcp);

		fp = fopen(index_name, "wb+");

		if (fp) {
			fwrite(&size, sizeof(uint32_t), 1, fp);

			encoded_byte_array = lz77_encode(text, size, Ls, Ll, &code_len);
			fwrite(encoded_byte_array, sizeof(uint8_t), code_len, fp);
			free(encoded_byte_array);

			byte_array = get_bytes_from_array(sarray, size);
			free(sarray);
			encoded_byte_array = lz77_encode(byte_array, size * 4, Ls, Ll, &code_len);
			free(byte_array);
			fwrite(encoded_byte_array, sizeof(uint8_t), code_len, fp);
			free(encoded_byte_array);

			byte_array = get_bytes_from_array(Llcp, size);
			free(Llcp);
			encoded_byte_array = lz77_encode(byte_array, size * 4, Ls, Ll, &code_len);
			free(byte_array);
			fwrite(encoded_byte_array, sizeof(uint8_t), code_len, fp);
			free(encoded_byte_array);

			byte_array = get_bytes_from_array(Rlcp, size);
			free(Rlcp);
			encoded_byte_array = lz77_encode(byte_array, size * 4, Ls, Ll, &code_len);
			free(byte_array);
			fwrite(encoded_byte_array, sizeof(uint8_t), code_len, fp);
			free(encoded_byte_array);

			fclose(fp);
		} else {
			printf("Erro ao abrir o arquivo %s\n", index_name);
			exit(1);
		}
	} else {
		printf("Erro ao abrir o arquivo %s\n", source_file);
		exit(1);
	}
}
