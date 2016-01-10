#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <vector>

unsigned char const INDEX_MODE = 0;
unsigned char const SEARCH_MODE = 1;
unsigned char const LZ77 = 0;
unsigned char const LZ78 = 1;

struct program_args {
  program_args();
  ~program_args();

  unsigned char mode_flag;
  unsigned char compression_flag;
  char* pattern_file;
  std::vector<std::string> patterns;
  bool help_flag;
  bool count_flag;
  char* index_file;
  char* text_file;
  uint32_t largest_pattern_length;
};

program_args get_program_parameters(int argc, char** argv);
void print_help_text();

void read_pattern_file(program_args &args);
void search_index_file(program_args &args);
int is_regular_file(const char *path);
void create_index_file(program_args &args);

#endif
