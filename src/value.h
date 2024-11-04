#ifndef nol_value_h
#define nol_value_h

#include "common.h"

typedef enum { VAL_BOOL, VAL_INT, VAL_FLOAT, VAL_CHAR } ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    int32_t integer;
    double floating;
  } as;
} Value;

#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define INT_VAL(value) ((Value){VAL_INT, {.integer = value}})
#define FLOAT_VAL(value) ((Value){VAL_FLOAT, {.floating = value}})

#define AS_BOOL(value) ((value).as.boolean)
#define AS_INT(value) ((value).as.integer)
#define AS_FLOAT(value) ((value).as.floating)

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_INT(value) ((value).type == VAL_INT)
#define IS_FLOAT(value) ((value).type == VAL_FLOAT)

bool valuesEqual(Value a, Value b);

#endif