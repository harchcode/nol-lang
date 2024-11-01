#ifndef nol_scanner_h
#define nol_scanner_h

typedef enum {
  // Single-character tokens.
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_MINUS,
  TOKEN_PLUS,
  TOKEN_SEMICOLON,
  TOKEN_SLASH,
  TOKEN_STAR,
  TOKEN_PERCENT,

  // One or two character tokens.
  TOKEN_BANG,
  TOKEN_BANG_EQUAL,
  TOKEN_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,
  TOKEN_AMP,
  TOKEN_AMP_AMP,
  TOKEN_PIPE,
  TOKEN_PIPE_PIPE,

  // Literals.
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_NUMBER,

  // Keywords.
  TOKEN_ELSE,
  TOKEN_FALSE,
  TOKEN_FOR,
  TOKEN_IF,
  TOKEN_PRINT,
  TOKEN_RETURN,
  TOKEN_TRUE,
  TOKEN_WHILE,
  TOKEN_INT,
  TOKEN_FLOAT,
  TOKEN_BOOL,
  TOKEN_CHAR,

  TOKEN_ERROR,
  TOKEN_EOF,
} Token;

void init_scanner(const char* source);
int get_scanner_line();
const char* get_scanner_start();
const char* get_scanner_current();

Token scan_token();

#endif