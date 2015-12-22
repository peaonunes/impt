#include <getopt.h>
#include <iostream>
#include <stdlib.h>

#include "utils.h"
#include "input/FileReader.h"

using namespace std;

int main (int argc, char **argv) {
  
  program_args args = get_program_parameters(argc, argv);

  if (args.help_flag) {
    print_help_text();
  } else if (args.mode_flag==1) { //Index mode
    cout << "Index mode" << endl;		
    if (args.text_file) {
      cout << "Arquivo de texto: " << args.text_file << endl;
      create_index_file(args);
    } else {
      cerr << "Nenhum arquivo de texto foi fornecido." << endl;
      exit(1);
    }
	} else if (args.mode_flag==2) {	//Search mode
    cout << "Search mode" << endl;
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
    if (args.index_file) {
      cout << "Arquivo de índices: " << args.index_file << endl;
      search_index_file(args);

      // Print results
    } else {
      cerr << "Nenhum arquivo de índice foi fornecido." << endl;
      exit(1);
    }
	} else { //Invalid mode
    cout << "Invalid mode" << endl;
    exit(1);
  }

  exit (0);
}
