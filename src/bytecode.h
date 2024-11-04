#ifndef nol_bytecode_h
#define nol_bytecode_h

#include "common.h"

typedef enum {
  OP_RETURN,
  OP_CONSTANT,
  OP_NEGATE,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_TRUE,
  OP_FALSE,
  OP_NOT,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS
} OP;

void init_code();
void free_code();
uint8_t* get_code();
int get_code_size();
void reserve_code(int size);
void write_code(uint8_t byte);
void write_value(void* src, int size);

#endif