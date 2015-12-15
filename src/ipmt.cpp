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
		// Check if textfile exists		
		create_index_file(args);
	} else if (args.mode_flag==2) {	//Search mode
		
	}
  /*  if (!args.quiet_flag) {
			cout << args.mode_flag << endl;
      cout << "Modo: " << (args.mode_flag==1 ? "index" : (args.mode_flag==2 ? "search" : "error")) << endl;

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
    
    if (args.index_file) {
      if (!args.quiet_flag) cout << "Arquivos de texto a serem buscados:" << endl;
      search_index_file(args);
    }
*/
    exit (0);
  }
}
