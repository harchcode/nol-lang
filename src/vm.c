#include "vm.h"

#include "bytecode.h"
#include "common.h"
#include "debug.h"
#include "value.h"

#define DEBUG_TRACE_EXECUTION

Value stack[STACK_MAX];
Value* top;

void reset_stack() { top = stack; }

void push(Value value) {
  *top = value;
  top++;
}

Value pop() {
  top--;
  return *top;
}

void init_vm() { reset_stack(); }
void free_vm() {}

// Value peek(int distance) { return top[-1 - distance]; }

bool is_falsy(Value value) { return (IS_BOOL(value) && !AS_BOOL(value)); }

bool run_code(uint8_t* code) {
#define BINARY_OP(value_type, op)         \
  do {                                    \
    Value b = pop();                      \
    Value a = pop();                      \
    int32_t res = AS_INT(a) op AS_INT(b); \
    push(value_type(res));                \
  } while (false);

  uint8_t* ip = code;

  while (true) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value* slot = stack; slot < top; slot++) {
      printf("[ ");
      switch (slot->type) {
        case VAL_BOOL:
          printf(AS_BOOL(*slot) ? "true" : "false");
          break;
        case VAL_INT:
          printf("%d", AS_INT(*slot));
          break;
        default:
          break;
      }
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
        int32_t integer;
        memcpy(&integer, ip, sizeof(int32_t));

        ip += sizeof(int32_t);

        push(INT_VAL(integer));

        break;
      }
      case OP_ADD:
        BINARY_OP(INT_VAL, +);
        break;
      case OP_SUBTRACT:
        BINARY_OP(INT_VAL, -);
        break;
      case OP_MULTIPLY:
        BINARY_OP(INT_VAL, *);
        break;
      case OP_DIVIDE:
        BINARY_OP(INT_VAL, /);
        break;
      case OP_NEGATE:
        push(INT_VAL(-AS_INT(pop())));
        break;
      case OP_TRUE:
        push(BOOL_VAL(true));
        break;
      case OP_FALSE:
        push(BOOL_VAL(false));
        break;
      case OP_NOT:
        push(BOOL_VAL(is_falsy(pop())));
        break;
      case OP_EQUAL: {
        Value b = pop();
        Value a = pop();
        push(BOOL_VAL(valuesEqual(a, b)));
        break;
      }
      case OP_GREATER:
        BINARY_OP(BOOL_VAL, >);
        break;
      case OP_LESS:
        BINARY_OP(BOOL_VAL, <);
        break;
      case OP_RETURN: {
        Value value = pop();

        switch (value.type) {
          case VAL_BOOL:
            printf(AS_BOOL(value) ? "true" : "false");
            break;
          case VAL_INT:
            printf("%d", AS_INT(value));
            break;
          default:
            break;
        }

        printf("\n");

        return true;
      }
    }
  }

  return true;

#undef BINARY_OP
}