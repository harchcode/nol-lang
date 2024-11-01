#include "bytecode.h"

#include "memory.h"

uint8_t* code;
int count;
int capacity;

void init_code() {
  code = NULL;
  count = 0;
  capacity = 0;
}

void free_code() {
  FREE_ARRAY(uint8_t, code, capacity);

  init_code();
}

uint8_t* get_code() { return code; }
int get_code_size() { return count; }

void write_code(uint8_t byte) {
  if (capacity < count + 1) {
    int old_capacity = capacity;

    capacity = GROW_CAPACITY(old_capacity);
    code = GROW_ARRAY(uint8_t, code, old_capacity, capacity);
  }

  code[count] = byte;
  count++;
}