#include <getopt.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>

#include "utils.h"
#include "input/FileReader.h"
#include "sarray/sarray.h"
#include "compression/lz78.h"

using namespace std;

program_args::program_args()
  : mode_flag(0),
    pattern_file(0),
    help_flag(false),
    quiet_flag(false),
    count_flag(false),
    index_file(0),
    text_file(0) { }

program_args::~program_args() {

}

program_args get_program_parameters(int argc, char** argv) {
  int option_index;
  int current_parameter;
  program_args args;
	
  struct option long_options[] =
  {
    {"pattern", required_argument, 0, 'p'},
    {"help",    no_argument,       0, 'h'},
    {"count",   no_argument,       0, 'c'},
    {0, 0, 0, 0}
  };
	
	if (argc > 1) {
		char* mode = argv[1];
		if (strcmp(mode, "index") == 0) {
 			args.mode_flag = 1;
			optind++;
		}
		else if (strcmp(mode, "search") == 0) {
      args.mode_flag = 2;
			optind++;
		}
	}

  while (1) {
    current_parameter = getopt_long(argc, argv, "p:hc", long_options, &option_index);

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
	
  if (args.mode_flag == 1) {
    if (optind < argc) {
      
      args.text_file = argv[optind++];
    }
  } else if (args.mode_flag == 2) {
    if (optind < argc) {   
      if (!args.pattern_file) {
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
      args.patterns.push_back(buffer);
    }
  }
}

void create_index_file(program_args &args) {
  // cout << "Creatng index file" << endl;
  ifstream fileStream(args.text_file);
  string text;
  if (fileStream.good()) {

    // Text length
    fileStream.seekg(0, ios::end);
    int length = fileStream.tellg();
    fileStream.seekg(0, ios::beg);
    
    // Assigning file to a string
    text.assign((istreambuf_iterator<char>(fileStream)),
                istreambuf_iterator<char>());
    fileStream.close();
    
    // cout << text << endl;
    
    char* writable = new char[text.size() + 1];
    std::copy(text.begin(), text.end(), writable);
    writable[text.size()] = '\0'; // don't forget the terminating 0

    int* sarray;
    int* Llcp;
    int* Rlcp;

    //build_sarray_LRlcp(writable, length, &sarray, &Llcp, &Rlcp);

    //cout << "Sarray done." << endl;

    // Compress and store in a file.

    encrypt(text);
    /*cout << "Encryption: " << endl;
    string code = encrypt(text);
    cout << code << endl;
    cout << "/Encryption: " << endl;
    cout << "Decryption: " << endl;
    string dec = decode(code);
    cout << dec << endl;
    cout << "/Decryption: " << endl;*/
    
    delete[] writable;

  } else {
    cerr << "Error reading text file." << endl;
    exit(1);
  }


}

void search_index_file(program_args &args) {
  // cout << "Searching index file" << endl;
  decode("");
}
