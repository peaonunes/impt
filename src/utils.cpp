#include <getopt.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <glob.h>
#include <stdio.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "utils.h"
#include "input/FileReader.h"

using namespace std;

program_args::program_args()
  : mode_flag(0),
    pattern_file(0),
    help_flag(false),
    quiet_flag(false),
    count_flag(false),
    index_file(0) { }

program_args::~program_args() {

}

program_args get_program_parameters(int argc, char** argv) {
  int option_index;
  int current_parameter;
  program_args args;
	
  struct option long_options[] =
  {
    /* These options set a flag */
    //{"index",   no_argument,       &args.mode_flag, 1},
    //{"search",  no_argument,       &args.mode_flag, 2},
    /* These options don’t set a flag.
             We distinguish them by their indices. */
    {"pattern", required_argument, 0, 'p'},
    {"help",    no_argument,       0, 'h'},
    {"quiet",   no_argument,       0, 'q'},
    {"count",   no_argument,       0, 'c'},
    {0, 0, 0, 0}
  };
	
	if (argc > 1) {
		cout << "argc=" << argc << endl;
		cout << "argv[1]=" << argv[1] << endl;
		char* mode = argv[1];
		if (strcmp(mode, "index")) {
			args.mode_flag = 1;
			optind++;
		}
		else if (strcmp(mode, "search")) {
			args.mode_flag = 2;
			optind++;
		}
	}

  while (1) {
    current_parameter = getopt_long(argc, argv, "p:hqc", long_options, &option_index);

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
      case 'p':
      args.pattern_file = optarg;
      break;
      case 'h':
      args.help_flag = true;
      break;
      case 'c':
      args.count_flag = true;
      break;
      case '?':
      // Um argumento desconhecido é apenas ignorado no momento
      break;
      default:
      exit(1);
    }
  }
	

	cout << "optind=" << optind << endl;
	
  if (optind < argc) {
    
    if (!args.pattern_file) {
      args.patterns.push_back(argv[optind++]);
    }
  } //else error?

  return args;
}

void print_help_line(char const *msg1, char const *msg2) {
	cout << left << setw(35) << msg1 << setw(45) << msg2 << endl;
}

void print_help_text() {
  cout << "Usage: ipmt mode ..." << endl;
  cout << "Options:" << endl;
  print_help_line("  -q, --quiet", "Inhibits every output message");
  print_help_line("  -p, --pattern=<pattern file>","Specifies file from which the program should read the patterns to be used (each line of the file specifies a pattern)");
  print_help_line("  -h, --help","Shows this message");
  cout << endl << "  If a pattern file is not specified, the first argument given to pmt will be read as the only pattern to be searched for in the text file. Several source text files can be specified at the same time." << endl;
}

int is_regular_file(const char *path) {
		struct stat path_stat;
		stat(path, &path_stat);
		return S_ISREG(path_stat.st_mode);
}

/* globerr --- print error message for glob() */

int glob_error(const char *path, int eerrno) {
	fprintf(stderr, "pmt: %s: %s\n", path, strerror(eerrno));
	return 0; /* let glob() keep going */
}

void read_pattern_file(program_args &args) {
  FileReader fr(args.pattern_file);
  string buffer;

  while(fr.hasContent()) {
    buffer = fr.readLine();

    if (buffer.size()) {
      args.patterns.push_back(buffer);
    }
  }
}

void create_index_file(program_args &args) {
  
}

/*
 * search_files --- searches source_text_files entries
 * whose name matches with one or more of the given
 * filenames
 */

void search_index_file(program_args &args) {
  
}
