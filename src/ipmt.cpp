#include <iostream>
#include <stdlib.h>

#include "utils.h"

using namespace std;

int main (int argc, char **argv) {

	program_args args = get_program_parameters(argc, argv);

	if (args.help_flag) {
		print_help_text();

	} else if (args.mode_flag==Index) {
		if (args.text_file) {
			create_index_file(args);

		} else {
			cerr << "Nenhum arquivo de texto foi fornecido." << endl;
			exit(1);

		}
	} else if (args.mode_flag==Search) {
		if (args.pattern_file) {
			read_pattern_file(args);

		} else {
			if (args.patterns.empty()) {
				cerr << "Nenhum padrão ou arquivo de padrões foi fornecido." << endl;
				exit(1);

			}
		}

		if (args.index_file) {
			search_index_file(args);

		} else {
			cerr << "Nenhum arquivo de índices foi fornecido." << endl;
			exit(1);

		}
	} else {
		cerr << "Modo inválido. Tente 'search' ou 'index'. Use -h ou --help para mais informações." << endl;
		exit(1);
	}
	exit (0);
}