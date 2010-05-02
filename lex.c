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

struct string {
  int i, m;
  char *a;
};

struct string string () {
  struct string s = { 0, 4, calloc (1, 4) };
  return s;
}

void spush (struct string *s, char c) {
  if (s->i == s->m-1) {
    s->m = s->m * 3 / 2;
    s->a = realloc (s->a, s->m);
  }
  s->a[s->i++] = c;
}

CaosValue caos_lexer_lex (CaosLexer *l) {
  int i;
  struct string s;

  if (!*l->p) return caos_value_eoi();

  char c = *l->p++;
  while (iswhitespace(c)) c = *l->p++;
  
   if (isdigit (c)) {
    i = 0;
    switch (c) {
    case '9': ++i;
    case '8': ++i;
    case '7': ++i;
    case '6': ++i;
    case '5': ++i;
    case '4': ++i;
    case '3': ++i;
    case '2': ++i;
    case '1': ++i;
    case '0':
      while (isdigit(*l->p)) {
        i *= 10;
        switch (*l->p++) {
        case '9': ++i;
        case '8': ++i;
        case '7': ++i;
        case '6': ++i;
        case '5': ++i;
        case '4': ++i;
        case '3': ++i;
        case '2': ++i;
        case '1': ++i;
        }
      }
      return caos_value_int (i);
    }
  }
  else if (isalpha (c)) {
    s = string();
    spush (&s, tolower(c));
    while (isalpha (*l->p) || *l->p == ':')
      spush (&s, tolower(*l->p++));
    return caos_value_symbol (s.a);
  }
  else if (c == '"') {
    s = string();
    while (*l->p != '"') spush (&s, *l->p++);
    l->p++; // Skip endquote
    return caos_value_string (s.a);
  }
  printf ("Error!\n");
  abort();
  return caos_value_null();
}
