#include "debug.h"

#include <stdio.h>

#include "bytecode.h"

void log_code() {
  uint8_t* code = get_code();
  int size = get_code_size();
  int offset = 0;

  while (offset < size) {
    log_instruction(code, &offset);
  }
}

void log_simple(const char* name, int* offset) {
  printf("%s\n", name);

  *offset += 1;
}

void log_constant(const char* name, uint8_t* code, int* offset) {
  int32_t value;

  memcpy(&value, &code[*offset + 1], sizeof(int32_t));

  printf("%-16s %d\n", name, value);

  *offset += 5;
}

void log_instruction(uint8_t* code, int* offset) {
  printf("%04d ", *offset);

  uint8_t instruction = code[*offset];

  switch (instruction) {
    case OP_CONSTANT:
      return log_constant("OP_CONSTANT", code, offset);
    case OP_ADD:
      return log_simple("OP_ADD", offset);
    case OP_SUBTRACT:
      return log_simple("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
      return log_simple("OP_MULTIPLY", offset);
    case OP_DIVIDE:
      return log_simple("OP_DIVIDE", offset);
    case OP_NEGATE:
      return log_simple("OP_NEGATE", offset);
    case OP_TRUE:
      return log_simple("OP_TRUE", offset);
    case OP_FALSE:
      return log_simple("OP_FALSE", offset);
    case OP_NOT:
      return log_simple("OP_NOT", offset);
    case OP_EQUAL:
      return log_simple("OP_EQUAL", offset);
    case OP_GREATER:
      return log_simple("OP_GREATER", offset);
    case OP_LESS:
      return log_simple("OP_LESS", offset);
    case OP_RETURN:
      return log_simple("OP_RETURN", offset);
    default:
      printf("Unknown opcode %d\n", instruction);
      *offset += 1;
      return;
  }
}