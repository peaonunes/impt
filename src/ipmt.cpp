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
  } else {
    if (!args.quiet_flag) {
      cout << "Modo de busca: ";

      if (args.allowed_edit_distance > 0) {
        cout << "aproximada (distancia máxima = " << args.allowed_edit_distance << ")" << endl;
      } else {
        cout << "exata" << endl;
      }

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
    } else {
      read_pattern_file(args);
    }
    
    if (args.source_text_files) {
      if (!args.quiet_flag) cout << "Arquivos de texto a serem buscados:" << endl;
      search_files(args);
    }

    exit (0);
  }
}
