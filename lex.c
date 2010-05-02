#include "lex.h"
#include "caos.h"
#include "dairy.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

CaosLexer caos_lexer (char *script) {
  CaosLexer lex = { script, script };
  return lex;
}

bool iswhitespace (char c) {
  switch (c) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    return true;
  default:
    return false;
  }
}

bool isnotstrchar (char c) {
  return c == '"';
}

bool isnotsymchar (char c) {
  return ! (isalnum(c) || c == ':');
}

char* lex_string (CaosLexer *l, int i, bool (*end) (char), char (*trans)(char)) {
  char *s;
  char c = *l->p;
  if (trans) c = trans (c);

  if (i > 1000) abort();
  
  if (end(c))
    s = calloc (1, i + 1);
  else {
    l->p++;
    s = lex_string (l, i + 1, end, trans);
    s[i] = c;
  }
  return s;
}

CaosValue caos_lexer_lex (CaosLexer *l) {
  int i;

  if (!*l->p) return caos_value_eoi();

  char c = *l->p++;
  while (iswhitespace(c)) c = *l->p++;
  
   if (isdigit (c)) {
    i = c - '0';
    while (isdigit(*l->p)) {
      i *= 10;
      i += *l->p++ - '0';
    }
    return caos_value_int (i);
  }
  else if (isalpha (c)) {
    l->p--;
    return caos_value_symbol (lex_string (l, 0, isnotsymchar, (char(*)(char))tolower));
  }
  else if (c == '"') {
    char *s = lex_string (l, 0, isnotstrchar, NULL);
    l->p++; // skip endquote
    return caos_value_string (s);
  }
  printf ("Error! '%c'\n", c);
  abort();
  return caos_value_null();
}
