#include "vm.h"

#include "bytecode.h"
#include "common.h"
#include "debug.h"

#define DEBUG_TRACE_EXECUTION

int32_t stack[STACK_MAX];
int32_t* top;

void reset_stack() { top = stack; }

void push(int32_t value) {
  *top = value;
  top++;
}

int32_t pop() {
  top--;
  return *top;
}

void init_vm() { reset_stack(); }
void free_vm() {}

bool run_code(uint8_t* code) {
#define BINARY_OP(op)  \
  do {                 \
    int32_t b = pop(); \
    int32_t a = pop(); \
    push(a op b);      \
  } while (false)

  uint8_t* ip = code;

  while (true) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (int32_t* slot = stack; slot < top; slot++) {
      printf("[ ");
      printf("%d", *slot);
      printf(" ]");
    }
    printf("\n");

    int offset = ip - code;
    log_instruction(code, &offset);
#endif

    uint8_t instruction = *ip;
    ip++;

    switch (instruction) {
      case OP_CONSTANT: {
        int32_t value;
        memcpy(&value, ip, sizeof(int32_t));

        ip += sizeof(int32_t);

        push(value);

        break;
      }
      case OP_ADD:
        BINARY_OP(+);
        break;
      case OP_SUBTRACT:
        BINARY_OP(-);
        break;
      case OP_MULTIPLY:
        BINARY_OP(*);
        break;
      case OP_DIVIDE:
        BINARY_OP(/);
        break;
      case OP_NEGATE:
        push(-pop());

        break;
      case OP_RETURN: {
        int32_t value = pop();

        printf("%d\n", value);

        return true;
      }
    }
  }

  return true;

#undef BINARY_OP
}