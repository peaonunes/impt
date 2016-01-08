#include <iostream>
#include <cstddef>
#include <stdio.h>
#include <string.h>

#include "lz78.h"

/* Dictionary/Tree structure for storing text entries occurrences */
class d_node {
public:
  /* Attributes */
  int label;
  char byte;
  d_node* parent;
  d_node* left;
  d_node* right;
  bool initialized;
  
  /* Methods */
  d_node();
  d_node(char c, int l, d_node* p);
  d_node* findEntry(char c);
  d_node* insertEntry(char byte, int label);
};

d_node::d_node() {
  initialized = false;
  left = NULL;
  right = NULL;
}

d_node::d_node(char c, int l, d_node* p) {
  initialized = true;
  byte = c;
  label = l;
  parent = p;
  left = NULL;
  right = NULL;
}

/* Searches for a determined node value in the dictionary tree */
d_node* d_node::findEntry(char c){
  if  ((&right) == NULL) return NULL;
  d_node* cur = right;
  while (cur != NULL) {
    if  (cur->byte == c) return cur;
    cur = cur->left;
  }
  return NULL;
}

/* Adds a new value to the dictionary tree */
d_node* d_node::insertEntry(char byte, int label){
  if(right == NULL || !right->initialized) {
    right = new d_node(byte,label,this);
    return right;
  } else {
    d_node* cur = right;
    while(cur->left != NULL && cur->left->initialized) {
      cur = cur->left;
    }
    cur->left = new d_node(byte,label,this);
    return cur->left;
  }
}

/***************** lz78 util functions *****************/

/* this first figures out how many bits it takes to write down max_label, and then writes label as a number using that many bits.  
  It stores in "buffer" things that haven't been written out yet.  
 */
void printLabel (int label, int max_label, uint8_t* &code, uint32_t &code_length) {
  unsigned char buffer = 0;
  int buffer_size = 0;
  int mask;

  if    (max_label == 0) return;
  for   (mask = 1; max_label > 1; max_label /=2) mask *= 2;
  
  for   (; mask != 0; mask /= 2) {
    buffer = buffer * 2 + ((label & mask) / mask);
    buffer_size++;
    
    if  (buffer_size == 8) {
      //std::cout.put(buffer);
      code[code_length] = buffer;
      code_length++;
      buffer = 0;
      buffer_size = 0;
    }
  }
}

/* same as above */
void printLetter (char c, uint8_t* &code, uint32_t &code_length) {
  printLabel((unsigned char) c, 128, code, code_length);
}

/* reads in labels written out using printLabel.
 */
int readLabel (int max_label) {
  int buffer;
  int buffer_size = 0;
  int label;

  for   (label=0; max_label != 0; max_label /= 2) {
    if  (buffer_size == 0) {
      buffer = fgetc(stdin);
      if  (buffer == EOF) return -1;
      buffer_size = 8;
    }
    label = label * 2 +  ((buffer & 128) / 128);
    buffer *= 2;
    buffer_size--;
  }
  return label;

}

/* use like cin.get(c). returns 1 normdicty, or 0 on end of file */
int readLetter (char &c) {
  int val = readLabel(128);
  if (val == -1) return 0;
  c = (char) val;
  return 1;
}

uint8_t* lz78_encode (char* text, int text_length, uint32_t* code_length){
  int max_label = (1 << 25);
  int dictCount = 1;
  d_node * head = new d_node(NULL, 0, (d_node *)NULL);
  d_node *cur = head;
  int i = 0;
  char c;
  uint8_t* code = (uint8_t*)malloc(3 * text_length * sizeof(uint8_t));
  uint32_t index_length = 0;
  while(i < text_length) {
    c = text[i];
    i++;
    d_node* dict_entry = cur->findEntry(c);
    if(dict_entry == NULL) {
      printLabel(cur->label, max_label, code, index_length);
      printLetter(c, code, index_length);
      cur->insertEntry(c, dictCount);
      dictCount++;
      cur = head;
    } else {
      cur = dict_entry;
    }
  }
  printLabel(cur->label, max_label, code, index_length);
  printLabel(0, 127, code, index_length);
  (*code_length) = index_length;
  return code;
}

d_node** dict;

std::string decodePath(d_node *last_node, bool error = false, bool newLine = false) {
  std::string str = "";
  if(error) {
    std::cerr << str;
  } else {
    while(last_node != NULL && last_node->parent != NULL) {
      str = last_node->byte + str;
      last_node = last_node->parent;
    }
    //std::cout << str;
    for (int i = 0; i < str.size(); ++i) {
      //write
    }
  }
  if(newLine) std::cerr << "\n";
  return str;
}

void expandArray(long size) {
  long newsize = size * 2;
  d_node** olddict = dict;
  dict = new d_node*[newsize];
  for(int i = 0; i < newsize; i++)
    dict[i] = NULL;
  for(int i = 0; i < size; i++) {
    if(olddict[i] == NULL) { continue; }
    dict[i] = olddict[i];
  }
}

char* lz78_decode (uint8_t* code) {
  int max_label = (1 << 25);
  int dictCount = 1;
  d_node* head = new d_node(NULL, 0, (d_node *)NULL);
  d_node* cur = head;
  std::string text = "";
  long size = 256;
  dict = new d_node*[size];
  dict[0] = head;
  int nextLabel;
  char c = NULL;
  char nextC = NULL;
  int label = readLabel(max_label);
  readLetter(c);
  while( (nextLabel = readLabel(max_label)) != -1 && readLetter(nextC) != -1 ) {
    if(label < size && dict[label] != NULL) {
      d_node* newNode;
      d_node* cur = dict[label];
      if( &c != NULL ) {
        newNode = cur->insertEntry(c, dictCount);
        while(dictCount >= size) {
          expandArray(size);
          size = size*2;
        }
        dict[dictCount] = newNode;
      } else {
        newNode = cur;
      }
      decodePath(newNode);
      dictCount++;
    }
    c = nextC;
    label = nextLabel;
    nextC = NULL;
  }
  
  if(label < size && dict[label] != NULL) {
    d_node* newNode;
    d_node* cur = dict[label];
    newNode = cur;
    decodePath(newNode);
    dictCount++;
  }

  return "";
}
