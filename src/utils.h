#ifndef UTILS_H
#define UTILS_H

#include <vector>

struct program_args {
  program_args();
  ~program_args();

  int mode_flag;
  char* pattern_file;
  std::vector<std::string> patterns;
  bool help_flag;
  bool quiet_flag;
  bool count_flag;
  char* index_file;
  char* text_file;
};

program_args get_program_parameters(int argc, char** argv);
void print_help_text();
int glob_error(const char *path, int eerrno);
void read_pattern_file(program_args &args);
void create_index_file(program_args &args);
void search_index_file(program_args &args);
int is_regular_file(const char *path);

#endif
