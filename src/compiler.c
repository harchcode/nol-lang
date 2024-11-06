#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>

#include "bytecode.h"
#include "common.h"
#include "debug.h"
#include "scanner.h"
#include "value.h"

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
} Prec;

typedef ValueType (*ParseFn)(ValueType prev_type);

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Prec precedence;
} ParseRule;

typedef struct {
  Token token;

  const char* start;
  const char* end;
  int line;
} ParseInfo;

typedef struct {
  ParseInfo current;
  ParseInfo previous;

  bool had_error;
  bool panic_mode;
} Parser;

Parser parser;

static ParseRule* get_rule(Token type);

void error_at(ParseInfo* info, const char* message) {
  if (parser.panic_mode) return;

  parser.panic_mode = true;
  fprintf(stderr, "[line %d] Error", info->line);

  if (info->token == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (info->token == TOKEN_ERROR) {
    // Nothing.
  } else {
    int len = info->end - info->start;
    fprintf(stderr, " at '%.*s'", len, info->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser.had_error = true;
}

void error_at_current(const char* message) {
  error_at(&parser.current, message);
}

void error(const char* message) { error_at(&parser.previous, message); }

void advance() {
  parser.previous = parser.current;

  while (true) {
    parser.current.token = scan_token();
    parser.current.start = get_scanner_start();
    parser.current.end = get_scanner_current();
    parser.current.line = get_scanner_line();

    if (parser.current.token != TOKEN_ERROR) break;

    error_at_current("ERROR!");
  }
}

void consume(Token token, const char* message) {
  if (parser.current.token == token) {
    advance();
    return;
  }

  error_at_current(message);
}

ValueType number() {
  int len = parser.previous.end - parser.previous.start;

  char* substr = malloc(len + 1);

  memcpy(substr, parser.previous.start, len);
  substr[len] = '\0';

  int32_t value = atoi(substr);

  free(substr);

  write_code(OP_CONSTANT);
  write_value(&value, sizeof(value));

  return VAL_INT;
}

ValueType literal() {
  switch (parser.previous.token) {
    case TOKEN_FALSE:
      write_code(OP_FALSE);
      return VAL_BOOL;
    case TOKEN_TRUE:
      write_code(OP_TRUE);
      return VAL_BOOL;
    default:
      return VAL_VOID;  // Unreachable.
  }
}

ValueType parse_prec(Prec precedence) {
  advance();
  ParseFn prefixRule = get_rule(parser.previous.token)->prefix;

  if (prefixRule == NULL) {
    error("Expect expression.");
    return VAL_VOID;
  }

  ValueType prefix_type = prefixRule(VAL_VOID);

  while (precedence <= get_rule(parser.current.token)->precedence) {
    advance();

    ParseFn infixRule = get_rule(parser.previous.token)->infix;
    prefix_type = infixRule(prefix_type);
  }

  return prefix_type;
}

ValueType expression() { return parse_prec(PREC_ASSIGNMENT); }

ValueType grouping() {
  ValueType value_type = expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
  return value_type;
}

bool is_number_type(ValueType val_type) { return val_type <= VAL_FLOAT; }

ValueType binary(ValueType left_type) {
  Token op = parser.previous.token;
  ParseRule* rule = get_rule(op);
  ValueType right_type = parse_prec((Prec)(rule->precedence + 1));

  switch (op) {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_STAR:
    case TOKEN_SLASH:
    case TOKEN_GREATER:
    case TOKEN_GREATER_EQUAL:
    case TOKEN_LESS:
    case TOKEN_LESS_EQUAL:
      if (!is_number_type(left_type) || !is_number_type(right_type)) {
        error("Expect a number.");
      } else if (left_type != right_type) {
        error("Expect operands to be of the same type.");
      }
      break;
    case TOKEN_BANG_EQUAL:
    case TOKEN_EQUAL_EQUAL:
      if (left_type != right_type) {
        error("Expect a matching type for equality comparison.");
      }
      break;
    default:
      break;
  }

  switch (op) {
    case TOKEN_PLUS:
      write_code(OP_ADD);
      return VAL_INT;
    case TOKEN_MINUS:
      write_code(OP_SUBTRACT);
      return VAL_INT;
    case TOKEN_STAR:
      write_code(OP_MULTIPLY);
      return VAL_INT;
    case TOKEN_SLASH:
      write_code(OP_DIVIDE);
      return VAL_INT;
    case TOKEN_BANG_EQUAL:
      write_code(OP_EQUAL);
      write_code(left_type);
      write_code(OP_NOT);
      return VAL_BOOL;
    case TOKEN_EQUAL_EQUAL:
      write_code(OP_EQUAL);
      write_code(left_type);
      return VAL_BOOL;
    case TOKEN_GREATER:
      write_code(OP_GREATER);
      return VAL_BOOL;
    case TOKEN_GREATER_EQUAL:
      write_code(OP_LESS);
      write_code(OP_NOT);
      return VAL_BOOL;
    case TOKEN_LESS:
      write_code(OP_LESS);
      return VAL_BOOL;
    case TOKEN_LESS_EQUAL:
      write_code(OP_GREATER);
      write_code(OP_NOT);
      return VAL_BOOL;
    default:
      return VAL_VOID;  // Unreachable.
  }
}

ValueType unary() {
  Token op = parser.previous.token;

  // Compile the operand.
  ValueType val_type = parse_prec(PREC_UNARY);

  // Emit the operator instruction.
  switch (op) {
    case TOKEN_MINUS:
      if (!is_number_type(val_type)) {
        error("Expect a number.");
      }
      write_code(OP_NEGATE);
      break;
    case TOKEN_BANG:
      if (val_type != VAL_BOOL) {
        error("Expect a boolean.");
      }
      write_code(OP_NOT);
      break;
    default:
      break;  // Unreachable.
  }

  return val_type;
}

ParseRule rules[TOKEN_EOF + 1] = {
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON}};

void init_rules() {
  for (int i = 0; i < TOKEN_EOF + 1; i++) {
    if (i == TOKEN_LEFT_PAREN || i == TOKEN_MINUS || i == TOKEN_PLUS ||
        i == TOKEN_SLASH || i == TOKEN_STAR || i == TOKEN_NUMBER ||
        i == TOKEN_TRUE || i == TOKEN_FALSE || i == TOKEN_BANG ||
        i == TOKEN_BANG_EQUAL || i == TOKEN_EQUAL_EQUAL || i == TOKEN_GREATER ||
        i == TOKEN_GREATER_EQUAL || i == TOKEN_LESS || i == TOKEN_LESS_EQUAL) {
      continue;
    }

    rules[i].prefix = NULL;
    rules[i].infix = NULL;
    rules[i].precedence = PREC_NONE;
  }
}

ParseRule* get_rule(Token type) { return &rules[type]; }

void compile(const char* source) {
  init_scanner(source);
  init_rules();

  parser.had_error = false;
  parser.panic_mode = false;

  free_code();

  advance();
  ValueType val_type = expression();
  consume(TOKEN_EOF, "Expect end of expression.");

  write_code(OP_RETURN);
  write_code(val_type);

  // log_code();
}