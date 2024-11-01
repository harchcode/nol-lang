#include "compiler.h"

#include <stdio.h>

#include "common.h"
#include "scanner.h"

// typedef enum {
//   PREC_NONE,
//   PREC_ASSIGNMENT,  // =
//   PREC_OR,          // or
//   PREC_AND,         // and
//   PREC_EQUALITY,    // == !=
//   PREC_COMPARISON,  // < > <= >=
//   PREC_TERM,        // + -
//   PREC_FACTOR,      // * /
//   PREC_UNARY,       // ! -
//   PREC_CALL,        // . ()
//   PREC_PRIMARY
// } Prec;

// typedef void (*ParseFn)();

// typedef struct {
//   ParseFn prefix;
//   ParseFn infix;
//   Prec precedence;
// } ParseRule;

// typedef struct {
//   Token token;

//   const char* start;
//   const char* end;
//   int line;
// } ParseInfo;

// typedef struct {
//   ParseInfo current;
//   ParseInfo previous;

//   bool had_error;
//   bool panic_mode;
// } Parser;

// typedef uint8_t* Program;

// Parser parser;
// Program code;

// static ParseRule* get_rule(Token type);

// static void error_at(ParseInfo& info, const char* message) {
//   if (parser.panic_mode) return;

//   parser.panic_mode = true;
//   fprintf(stderr, "[line %d] Error", info.line);

//   if (info.token == Token::END_OF_FILE) {
//     fprintf(stderr, " at end");
//   } else if (info.token == Token::ERROR) {
//     // Nothing.
//   } else {
//     string msg = string(info.start, info.end);
//     fprintf(stderr, " at '%s'", msg.c_str());
//   }

//   fprintf(stderr, ": %s\n", message);
//   parser.had_error = true;
// }

// static void error_at_current(const char* message) {
//   error_at(parser.current, message);
// }

// static void error(const char* message) { error_at(parser.previous, message);
// }

// static void advance() {
//   parser.previous = parser.current;

//   while (true) {
//     parser.current.token = scanner.next_token();
//     parser.current.start = scanner.start;
//     parser.current.end = scanner.current;
//     parser.current.line = scanner.line;

//     if (parser.current.token != Token::ERROR) break;

//     error_at_current("ERROR!");
//   }
// }

// static void consume(Token token, const char* message) {
//   if (parser.current.token == token) {
//     advance();
//     return;
//   }

//   error_at_current(message);
// }

// static void number() {
//   int value = stoi(string(parser.previous.start, parser.previous.end));

//   code.push_back(OP::CONSTANT);

//   uint8_t* bytep = static_cast<uint8_t*>(static_cast<void*>(&value));

//   for (int i = 0; i < sizeof(value); i++) {
//     code.push_back(bytep[i]);
//   }
// }

// static void parse_prec(Prec precedence) {
//   advance();
//   ParseFn prefixRule = get_rule(parser.previous.token)->prefix;

//   if (prefixRule == NULL) {
//     error("Expect expression.");
//     return;
//   }

//   prefixRule();

//   while (precedence <= get_rule(parser.current.token)->precedence) {
//     advance();

//     ParseFn infixRule = get_rule(parser.previous.token)->infix;
//     infixRule();
//   }
// }

// static void expression() { parse_prec(PREC_ASSIGNMENT); }

// static void grouping() {
//   expression();
//   consume(Token::RIGHT_PAREN, "Expect ')' after expression.");
// }

// static void binary() {
//   Token op = parser.previous.token;
//   ParseRule* rule = get_rule(op);
//   parse_prec((Prec)(rule->precedence + 1));

//   switch (op) {
//     case Token::PLUS:
//       code.push_back(OP::ADD);
//       break;
//     case Token::MINUS:
//       code.push_back(OP::SUBTRACT);
//       break;
//     case Token::STAR:
//       code.push_back(OP::MULTIPLY);
//       break;
//     case Token::SLASH:
//       code.push_back(OP::DIVIDE);
//       break;
//     default:
//       return;  // Unreachable.
//   }
// }

// static void unary() {
//   Token op = parser.previous.token;

//   // Compile the operand.
//   parse_prec(PREC_UNARY);

//   // Emit the operator instruction.
//   switch (op) {
//     case Token::MINUS:
//       code.push_back(OP::NEGATE);
//       break;
//     default:
//       return;  // Unreachable.
//   }
// }

// unordered_map<Token, ParseRule> rules = {
//     {Token::LEFT_PAREN, {grouping, NULL, PREC_NONE}},
//     {Token::MINUS, {unary, binary, PREC_TERM}},
//     {Token::PLUS, {NULL, binary, PREC_TERM}},
//     {Token::SLASH, {NULL, binary, PREC_FACTOR}},
//     {Token::STAR, {NULL, binary, PREC_FACTOR}},
//     {Token::NUMBER, {number, NULL, PREC_NONE}},
// };
// ParseRule empty_rule = {NULL, NULL, PREC_NONE};

// static ParseRule* get_rule(Token type) {
//   if (rules.find(type) != rules.end()) return &rules[type];

//   return &empty_rule;
// }

void compile(const char* source) {
  init_scanner(source);

  int line = -1;

  for (;;) {
    Token token = scan_token();

    int scanner_line = get_scanner_line();
    const char* start = get_scanner_start();
    const char* current = get_scanner_current();
    int len = current - start;

    if (scanner_line != line) {
      printf("%4d ", scanner_line);
      line = scanner_line;
    } else {
      printf("   | ");
    }

    if (token == TOKEN_EOF) {
      printf("EOF\n");

      break;
    } else if (token == TOKEN_ERROR) {
      printf("Unexpected Token.\n");
    } else {
      printf("%2d '%.*s'\n", token, len, start);
    }
  }
}