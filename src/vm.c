#include "vm.h"

#include "bytecode.h"
#include "common.h"
#include "debug.h"
#include "value.h"

#define DEBUG_TRACE_EXECUTION

uint8_t stack[STACK_MAX];
uint8_t* top;

void reset_stack() { top = stack; }

#define push(value_type, value)              \
  do {                                       \
    memcpy(top, &value, sizeof(value_type)); \
    top += sizeof(value_type);               \
  } while (false);

#define pop(value_type, out)               \
  do {                                     \
    top -= sizeof(value_type);             \
    memcpy(&out, top, sizeof(value_type)); \
  } while (false);

void init_vm() { reset_stack(); }
void free_vm() {}

// Value peek(int distance) { return top[-1 - distance]; }

// bool is_falsy(Value value) { return (IS_BOOL(value) && !AS_BOOL(value)); }

bool run_code(uint8_t* code) {
#define BINARY_OP(value_type, op) \
  do {                            \
    value_type b;                 \
    pop(value_type, b);           \
    value_type a;                 \
    pop(value_type, a);           \
    value_type res = a op b;      \
    push(value_type, res);        \
  } while (false);

  uint8_t* ip = code;

  while (true) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          [");
    for (uint8_t* slot = stack; slot < top; slot++) {
      if (slot == stack)
        printf("%d", *slot);
      else
        printf(", %d", *slot);
    }
    printf("]\n");

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

        push(int32_t, integer);

        break;
      }
      case OP_ADD:
        BINARY_OP(int32_t, +);
        break;
      case OP_SUBTRACT:
        BINARY_OP(int32_t, -);
        break;
      case OP_MULTIPLY:
        BINARY_OP(int32_t, *);
        break;
      case OP_DIVIDE:
        BINARY_OP(int32_t, /);
        break;
      case OP_NEGATE: {
        int32_t v;
        pop(int32_t, v);
        int32_t r = -v;
        push(int32_t, r);
        break;
      }
      case OP_TRUE: {
        bool v = true;
        push(bool, v);
        break;
      }
      case OP_FALSE: {
        bool v = false;
        push(bool, v);
        break;
      }
      case OP_NOT: {
        bool x;
        pop(bool, x);
        bool r = x == false;
        push(bool, r);
        break;
      }
      case OP_EQUAL: {
        ValueType operand_type;
        memcpy(&operand_type, ip, sizeof(ValueType));

        ip += sizeof(ValueType);

        switch (operand_type) {
          case VAL_INT: {
            int32_t b;
            pop(int32_t, b);
            int32_t a;
            pop(int32_t, a);
            bool r = a == b;
            push(bool, r);
            break;
          }
          case VAL_BOOL: {
            bool b;
            pop(bool, b);
            bool a;
            pop(bool, a);
            bool r = a == b;
            push(bool, r);
            break;
          }
          default:
            break;
        }

        break;
      }
      case OP_GREATER:
        BINARY_OP(bool, >);
        break;
      case OP_LESS:
        BINARY_OP(bool, <);
        break;
      case OP_RETURN: {
        bool value;
        pop(bool, value);

        printf(value ? "true" : "false");
        printf("\n");

        return true;
      }
    }
  }

  return true;

#undef BINARY_OP
}