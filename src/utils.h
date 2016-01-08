#ifndef UTILS_H
#define UTILS_H

#include <vector>

enum Mode { Index, Search };
enum Compression { LZ77, LZ78 };

struct program_args {
  program_args();
  ~program_args();

  Mode mode_flag;
  Compression compression_flag;
  char* pattern_file;
  std::vector<std::string> patterns;
  bool help_flag;
  bool count_flag;
  char* index_file;
  char* text_file;
};

program_args get_program_parameters(int argc, char** argv);
void print_help_text();

void read_pattern_file(program_args &args);
void search_index_file(program_args &args);
int is_regular_file(const char *path);
void create_index_file(char* source_file);

#endif
