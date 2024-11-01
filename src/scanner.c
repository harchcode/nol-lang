#include "scanner.h"

#include "common.h"
#include "stdio.h"

const char* start;
const char* current;
int line;

void init_scanner(const char* source) {
  start = source;
  current = source;
  line = 1;
}

int get_scanner_line() { return line; }
const char* get_scanner_start() { return start; }
const char* get_scanner_current() { return current; }

bool is_eof() { return *current == '\0'; }

bool match(char expected) {
  if (is_eof()) return false;
  if (*current != expected) return false;

  current++;

  return true;
}

char peek_next() {
  if (is_eof()) return '\0';
  return current[1];
}

void skip_whitespace() {
  while (true) {
    switch (*current) {
      case ' ':
      case '\r':
      case '\t':
        current++;
        break;
      case '\n':
        line++;
        current++;
        break;
      case '/':
        if (peek_next() == '/') {
          // A comment goes until the end of the line.
          while (*current != '\n' && !is_eof()) current++;
        } else {
          return;
        }
        break;
      default:
        return;
    }
  }
}

Token string_token() {
  while (*current != '"' && !is_eof()) {
    if (*current == '\n') line++;
    current++;
  }

  if (is_eof()) return TOKEN_ERROR;

  // The closing quote.
  current++;

  return TOKEN_STRING;
}

Token number_token() {
  while (isdigit(*current)) current++;

  // Look for a fractional part.
  if (*current == '.' && isdigit(peek_next())) {
    // Consume the ".".
    current++;

    while (isdigit(*current)) current++;
  }

  return TOKEN_NUMBER;
}

Token check_keyword(int check_start, int length, const char* rest,
                    Token token) {
  if (current - start == check_start + length &&
      memcmp(&start + check_start, rest, length) == 0) {
    return token;
  }

  return TOKEN_IDENTIFIER;
}

Token identifier_type() {
  switch (*start) {
    case 'b':
      return check_keyword(1, 3, "ool", TOKEN_BOOL);
    case 'c':
      return check_keyword(1, 3, "har", TOKEN_CHAR);
    case 'e':
      return check_keyword(1, 3, "lse", TOKEN_ELSE);
    case 'f':
      switch (start[1]) {
        case 'a':
          return check_keyword(2, 3, "lse", TOKEN_FALSE);
        case 'o':
          return check_keyword(2, 1, "r", TOKEN_FOR);
        case 'l':
          return check_keyword(2, 3, "oat", TOKEN_FLOAT);
      }

      break;
    case 'i':
      switch (start[1]) {
        case 'f':
          return TOKEN_IF;
        case 'n':
          return check_keyword(2, 1, "t", TOKEN_INT);
      }

      break;
    case 'p':
      return check_keyword(1, 4, "rint", TOKEN_PRINT);
    case 'r':
      return check_keyword(1, 5, "eturn", TOKEN_RETURN);
    case 't':
      switch (start[1]) {
        case 'r':
          return check_keyword(2, 2, "ue", TOKEN_TRUE);
      }

      break;
    case 'w':
      return check_keyword(1, 4, "hile", TOKEN_WHILE);
  }

  return TOKEN_IDENTIFIER;
}

Token identifier_token() {
  while (isalpha(*current) || isdigit(*current)) current++;
  return identifier_type();
}

Token scan_token() {
  skip_whitespace();

  start = current;

  if (is_eof()) return TOKEN_EOF;

  char c = *current;
  current++;

  if (isalpha(c)) return identifier_token();
  if (isdigit(c)) return number_token();

  switch (c) {
    case '(':
      return TOKEN_LEFT_PAREN;
    case ')':
      return TOKEN_RIGHT_PAREN;
    case '{':
      return TOKEN_LEFT_BRACE;
    case '}':
      return TOKEN_RIGHT_BRACE;
    case ';':
      return TOKEN_SEMICOLON;
    case ',':
      return TOKEN_COMMA;
    case '.':
      return TOKEN_DOT;
    case '-':
      return TOKEN_MINUS;
    case '+':
      return TOKEN_PLUS;
    case '/':
      return TOKEN_SLASH;
    case '*':
      return TOKEN_STAR;
    case '%':
      return TOKEN_PERCENT;

    case '!':
      return match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG;
    case '=':
      return match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL;
    case '<':
      return match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS;
    case '>':
      return match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER;
    case '&':
      return match('&') ? TOKEN_AMP_AMP : TOKEN_AMP;
    case '|':
      return match('|') ? TOKEN_PIPE_PIPE : TOKEN_PIPE;

    case '"':
      return string_token();
  }

  return TOKEN_ERROR;
}
