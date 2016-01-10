#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lz78.h"

/* Dictionary/Tree structure for storing text entries occurrences. Labels identifies a unique tree node, which stores a character. To get a word, find an entry and get it's parents until the root. */
class Dictionary_Tree {
public:
  int label;
  char byte;
  Dictionary_Tree* parent;
  Dictionary_Tree* left;
  Dictionary_Tree* right;
  bool is_empty;

  Dictionary_Tree();
  Dictionary_Tree(char c, int l, Dictionary_Tree* p);
  Dictionary_Tree* findEntry(char c);
  Dictionary_Tree* insertEntry(char byte, int label);
};

Dictionary_Tree::Dictionary_Tree() {

  is_empty = false;
  left = NULL;
  right = NULL;
}

Dictionary_Tree::Dictionary_Tree(char c, int l, Dictionary_Tree* p) {

  is_empty = true;
  byte = c;
  label = l;
  parent = p;
  left = NULL;
  right = NULL;
}

/* Searches for a determined node value in the dictionary tree */
Dictionary_Tree* Dictionary_Tree::findEntry(char c){

  if  ((&right) == NULL) return NULL;
  Dictionary_Tree* cur = right;

  while (cur != NULL) {
    if  (cur->byte == c) return cur;
    cur = cur->left;
  }

  return NULL;
}

/* Adds a new value to the dictionary tree. */
Dictionary_Tree* Dictionary_Tree::insertEntry(char byte, int label){
  if(right == NULL || !right->is_empty) {
    right = new Dictionary_Tree(byte,label,this);
    return right;
  } else {
    Dictionary_Tree* cur = right;
    while(cur->left != NULL && cur->left->is_empty) {
      cur = cur->left;
    }
    cur->left = new Dictionary_Tree(byte,label,this);
    return cur->left;
  }
}

/***************** lz78 encoding functions *****************/

/* this first figures out how many bits it takes to write down max_label, and then writes label as a number using that many bits.
  It stores in "buffer" things that haven't been written out yet.
 */
void writeLabel (int label, int max_label, uint8_t* &code, uint32_t &code_length, unsigned char &buffer, int &buffer_size) {

  int mask;

  if    (max_label == 0) return;
  for   (mask = 1; max_label > 1; max_label /=2) mask *= 2;

  for   (; mask != 0; mask /= 2) {
    buffer = buffer * 2 + ((label & mask) / mask);
    buffer_size++;

    if  (buffer_size == 8) {
      //std::cout.put(buffer);
      code[code_length++] = buffer;
      buffer = 0;
      buffer_size = 0;
    }
  }
}

/* same as above, buf for letters */
void writeLetter (char c, uint8_t* &code, uint32_t &code_length, unsigned char &buffer, int &buffer_size) {
  writeLabel((unsigned char) c, 128, code, code_length, buffer, buffer_size);
}

uint8_t* lz78_encode (char* text, size_t text_length, size_t* code_length){
  int max_label = (1 << 25);
  int dictCount = 1;

  uint32_t temp_code_length = 0;
  int index = 0;
  char c;

  uint8_t* code = (uint8_t*)malloc(3 * text_length * sizeof(uint8_t));

  Dictionary_Tree * head = new Dictionary_Tree(NULL, 0, (Dictionary_Tree *)NULL);
  Dictionary_Tree *cur = head;

  unsigned char buffer = 0;
  int buffer_size = 0;

  while(index < text_length) {

    c = text[index++];
    Dictionary_Tree* dict_entry = cur->findEntry(c);

    if(dict_entry == NULL) {

      writeLabel(cur->label, max_label, code, temp_code_length, buffer, buffer_size);
      writeLetter(c, code, temp_code_length, buffer, buffer_size);

      cur->insertEntry(c, dictCount);
      dictCount++;
      cur = head;
    } else {

      cur = dict_entry;
    }
  }
  writeLabel(cur->label, max_label, code, temp_code_length, buffer, buffer_size);
  writeLabel(0, 127, code, temp_code_length, buffer, buffer_size);

  (*code_length) = temp_code_length;
  return code;
}

/***************** lz78 decoding functions *****************/

/* reads in labels written out using writeLabel.
 */
int readLabel (int max_label, uint8_t* code, uint32_t code_length, int &index, int &buffer, int &buffer_size) {

  int label;

  for   (label=0; max_label != 0; max_label /= 2) {
    if  (buffer_size == 0) {
      if (index >= code_length) return -1;
      buffer = code[index++];;
      buffer_size = 8;
    }

    label = label * 2 +  ((buffer & 128) / 128);
    buffer *= 2;
    buffer_size--;
  }

  return label;
}

/* reads in letters written out using writeLetter.
 */
int readLetter (char &c, uint8_t* code, uint32_t code_length, int &index, int &buffer, int &buffer_size) {

  int val = readLabel(128, code, code_length, index, buffer, buffer_size);
  if (val == -1) return 0;
  c = (char) val;

  return 1;
}

/* Runs through the dict tree upwards to recover the encoded text from a given entry */
void getDecodedEntry(Dictionary_Tree *last_node, char* &text, int &index) {

  std::string str = "";

  while(last_node != NULL && last_node->parent != NULL) {
    str = last_node->byte + str;
    last_node = last_node->parent;
  }
  for (int i = 0; i < str.size(); ++i) text[index++] = str[i];
}

/* changes the dictionary size */
void expandDictionary(Dictionary_Tree** &dict, long size) {
  long newsize = size * 2;
  Dictionary_Tree** olddict = dict;
  dict = new Dictionary_Tree*[newsize];
  for(int i = 0; i < newsize; i++)
    dict[i] = NULL;
  for(int i = 0; i < size; i++) {
    if(olddict[i] == NULL) { continue; }
    dict[i] = olddict[i];
  }
}

char* lz78_decode (uint8_t* code, size_t code_length, size_t text_length) {

  int max_label = (1 << 25);
  int dictCount = 1;

  Dictionary_Tree* head = new Dictionary_Tree(NULL, 0, (Dictionary_Tree *)NULL);
  Dictionary_Tree* cur = head;

  /* Initial dict size */
  long size = 256;

  Dictionary_Tree** dict = new Dictionary_Tree*[size];
  dict[0] = head;

  char* text = (char*)malloc(text_length * sizeof(char));
  int text_index = 0;
  int code_index = 0;


  int nextLabel;
  char c = NULL;
  char nextC = NULL;

  int buffer;
  int buffer_size = 0;

  int label = readLabel(max_label, code, code_length, code_index, buffer, buffer_size);
  readLetter(c, code, code_length, code_index, buffer, buffer_size);

  while((nextLabel = readLabel(max_label, code, code_length, code_index, buffer, buffer_size)) != -1
        && readLetter(nextC, code, code_length, code_index, buffer, buffer_size) != -1 ) {

    if(label < size && dict[label] != NULL) {

      Dictionary_Tree* newNode;
      Dictionary_Tree* cur = dict[label];

      if( &c != NULL ) {

        newNode = cur->insertEntry(c, dictCount);

        while(dictCount >= size) {

          expandDictionary(dict, size);
          size = size*2;
        }

        dict[dictCount] = newNode;
      } else {

        newNode = cur;
      }

      getDecodedEntry(newNode, text, text_index);
      dictCount++;
    }

    c = nextC;
    label = nextLabel;
    nextC = NULL;
  }

  if(label < size && dict[label] != NULL) {

    Dictionary_Tree* newNode;
    Dictionary_Tree* cur = dict[label];
    newNode = cur;
    getDecodedEntry(newNode, text, text_index);
    dictCount++;
  }

  return text;
}
