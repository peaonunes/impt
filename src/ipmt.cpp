#include <iostream>
#include <stdlib.h>

#include "utils.h"

using namespace std;

int main (int argc, char **argv) {
	if (argc < 2) {
		printf("Uma ação - 'index' ou 'search' deve ser informada, bem como seus argumentos.\n");
		printf("Use a opção de ajuda (--help ou -h) para mais informações\n");
		exit(1);
	}

	char* action = argv[1];
	int indexActionSelected = !strcmp(action, "index");
	int searchActionSelected = !strcmp(action, "search");

	if (!indexActionSelected && !searchActionSelected) {
		printf("Uma ação - 'index' ou 'search' deve ser informada, bem como seus argumentos.\n");
		printf("Use a opção de ajuda (--help ou -h) para mais informações\n");
		exit(1);
	}

	program_args args = get_program_parameters(argc, argv);

	if (args.help_flag) {
		print_help_text();
	} else {
		if (!args.quiet_flag) {
			if (searchActionSelected) {
				if (args.pattern_file) {
					cout << "Arquivo de padrões: " << args.pattern_file << endl;

					read_pattern_file(args);
				} else {
					cout << "Nenhum arquivo de padrões foi fornecido." << endl;

					if (args.patterns.empty()) {
						cerr << "Nenhum padrão foi fornecido." << endl;
						exit(1);
					}
				}

				cout << "Padrões fornecidos:" << endl;
				for (int i = 0; i < args.patterns.size(); i++) {
					cout << "  " << args.patterns[i] << endl;
				}
			}
		} else {
			if (searchActionSelected) {
				if (args.pattern_file)
					read_pattern_file(args);
				else if (args.patterns.empty())
					exit(1);
			}
		}

		if (args.source_index_file) {
			if (indexActionSelected) {
				create_index_file(args.source_index_file);
			} else {
				printf("Falta implementar a busca\n");
			}
		}

		exit (0);
	}
}
