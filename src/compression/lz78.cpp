#include <iostream>
#include <cstddef>
#include <stdio.h>
#include <string.h>

#include "lz78.h"
#include "d_node.h"

void print_label (int label, int max_label) {
  unsigned char buffer = 0;
  int buffer_size = 0;
  int mask;

  if    (max_label == 0) return;
  for   (mask = 1; max_label > 1; max_label /=2) mask *= 2;
  
  for   (; mask != 0; mask /= 2) {
    buffer = buffer * 2 + ((label & mask) / mask);
    buffer_size++;
    
    if  (buffer_size == 8) {
      std::cout.put(buffer);
      buffer = 0;
      buffer_size = 0;
    }
  }
}

void print_letter (char c) {
  print_label((unsigned char) c, 128);
}

void print_final_label (int label, int max_label) {
  print_label(label, max_label);
  print_label(0, 127);
}

int read_label (int max_label) {
  static int buffer;
  static int how_full = 0;
  int label;

  for   (label=0; max_label != 0; max_label /= 2) {
    if  (how_full == 0) {
      buffer = fgetc(stdin);
      if  (buffer == EOF) return -1;
      how_full = 8;
    }
    label = label * 2 +  ((buffer & 128) / 128);
    buffer *=2;
    how_full--;
  }
  return label;

}

int read_letter (char &c) {
  int val = read_label(128);
  if (val == -1) return 0;
  c = (char) val;
  return 1;
}

std::string lz78_encode (char* text){
  int max_label = (1 << 25);
  int count = 1;
  d_node * head = new d_node(NULL, 0, (d_node *)NULL);
  d_node *cur = head;
  int i = 0;
  char c;
  char* code;
  int text_length = strlen(text);
  while(i < text_length) {
    c = text[i];
    i++;
    d_node* dict_entry = cur->find_entry(c);
    if(dict_entry == NULL) {
      print_label(cur->label, max_label);
      print_letter(c);
      cur->insert_entry(c, count);
      count++;
      cur = head;
    } else {
      cur = dict_entry;
    }
  }
  print_final_label(cur->label, max_label);
  return "";
}

d_node **all;

std::string decode_path(d_node *last_node, bool error = false, bool newLine = false) {
  std::string str = "";
  while(last_node != NULL && last_node->parent != NULL) {
    str =last_node->byte + str;
    last_node = last_node->parent;
  }
  if(error) std::cerr << str;
  else std::cout << str;
  if(newLine) std::cerr << "\n";
  return str;
}

void expand_array(long size) {
  long newsize = size * 2;
  d_node **oldAll = all;
  all = new d_node*[newsize];
  for(int i = 0; i < newsize; i++)
    all[i] = NULL;
  for(int i = 0; i < size; i++) {
    if(oldAll[i] == NULL) { continue; }
    all[i] = oldAll[i];
  }
}

std::string lz78_decode (std::string code) {
  int max_label = (1 << 25);
  int count = 1;
  d_node * head = new d_node(NULL, 0, (d_node *)NULL);
  d_node *cur = head;
  
  long size = 256;
  all = new d_node*[size];
  all[0] = head;
  int nextLabel;
  char c = NULL;
  char nextC = NULL;
  int label = read_label(max_label);
  read_letter(c);
  while( (nextLabel = read_label(max_label)) != -1 && read_letter(nextC) != -1 ) {
    if(label < size && all[label] != NULL) {
      d_node * newN;
      d_node * cur = all[label];
      if( &c == NULL ) {
        newN = cur;
      } else {
        newN = cur->insert_entry(c, count);
        while(count >= size) {
          expand_array(size);
          size = size*2;
        }
        all[count] = newN;
      }
      decode_path(newN);
      count++;
    }
    c = nextC;
    label = nextLabel;
    nextC = NULL;
  }
  
  if(label < size && all[label] != NULL) {
    d_node * newN;
    d_node * cur = all[label];
    newN = cur;
    decode_path(newN);
    count++;
  }
}
