#ifndef UTILS_H
#define UTILS_H

#include <vector>

struct program_args {
  program_args();
  ~program_args();

  char* pattern_file;
  std::vector<std::string> patterns;
  bool help_flag;
  bool quiet_flag;
  bool count_mode;
  char* source_index_file;
};

program_args get_program_parameters(int argc, char** argv);
void print_help_text();
void read_pattern_file(program_args &args);
void create_index_file(char* source_file);

#endif
