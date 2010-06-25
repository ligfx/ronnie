#include "ronnie.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// ~ Lexer ~
//

CaosLexer caos_lexer (enum CaosLexerVersion version, char *script) {
  CaosLexer lex = { script, script, version };
  return lex;
}

// Character classes

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

bool iseol (char c) { return '\n' == c || '\r' == c; }
bool isrightbracket (char c) { return ']' == c; }
bool isdoublequote (char c) { return '"' == c; }

bool issymchar (char c) {
  // $#a-zA-Z0-9:?!_+-
  if (isalnum(c)) return true;
  // TODO: Can we compare with a range?
  switch (c) {
  case '$':
  case '#':
  case ':':
  case '?':
  case '!':
  case '_':
  case '*':
  case '-':
  case '+':
    return true;
  default:
    return false;
  }
}

// Helpers

#ifndef strndup
char* strndup (char const *s, size_t n)
{
  char *new = malloc (n + 1);
  if (!new) return new;

  new[n] = '\0';
  return memcpy (new, s, n);
}
#endif

void advance_while (CaosLexer *l, bool (*func)(char))
{
  while (func (*l->p)) l->p++;
}

void advance_until (CaosLexer *l, bool (*func)(char))
{
  while (!func (*l->p)) l->p++;
}

void skip_whitespace (CaosLexer *l)
{
  advance_while (l, &iswhitespace);
}

bool skip_comment (CaosLexer *l)
{
  if ('*' == *l->p) {
    advance_until (l, &iseol);
    return true;
  }
  return false;
}

void skip_whitespace_and_comments (CaosLexer *l)
{
start:
  skip_whitespace (l);
  if (skip_comment (l)) goto start;
}

CaosValue lex_number (CaosLexer *l) {
  char *basep = l->p;
  
  if ('+' == *l->p || '-' == *l->p) l->p++;
  while (isdigit (*l->p)) l->p++;
//  advance_while (l, (bool(*)(char))&isdigit);
  if ('.' == *l->p++) {
    while (isdigit (*l->p)) l->p++;
//    advance_while (l, (bool(*)(char))&isdigit);
  
    printf ("[lexer] float: %f\n", atof(basep));
    return caos_value_float (atof (basep));
  } else {
    printf ("[lexer] int: %i\n", atoi(basep));
    return caos_value_int (atoi (basep));
  }
}

CaosValue lex_integer_character (CaosLexer *l) {
  assert ('\'' == *l->p++);
  int i = *l->p++;
  assert ('\'' == *l->p++);
  printf ("[lexer] int: %i\n", i);
  return caos_value_int(i);
}

CaosValue lex_integer_binary (CaosLexer *l) {
  int i = 0;
  assert ('%' == *l->p++);
  while ('0' == *l->p || '1' == *l->p) {
    i = (i << 1) + (*l->p++ == '1');
  }
  printf ("[lexer] int: %i\n", i);
  return caos_value_int(i);
}

CaosValue lex_symbol (CaosLexer *l)
{
  char *basep = l->p;
  advance_while (l, &issymchar);
  
  char *sym = NULL;
  size_t n = 0;
  // Binomials
  if (':' != *(l->p - 1)) {
    n = l->p - basep;
    sym = strndup (basep, n);
  } else {
    /*
      Okay, yes, this is horribly ugly.
      But it works?
      Input: "nEW: \n   \t siMp"
      Output: "new: simp"
      Yay!
   
      The point of this hackery is that things are easier
        when binomials are just a single symbol.
      Example: We can optimize caos.c by replacing function tokens with function
        pointers, because any function is implemented by A SINGLE TOKEN.
      Brilliant (maybe?)
    */
    char *a = l->p;
    skip_whitespace(l);
    char *b = l->p;
    advance_while (l, &issymchar);
    n = a - basep + l->p - b + 1;
    sym = malloc (n);
    sym[n] = 0;
    memcpy (sym, basep, a - basep);
    sym[a - basep] = ' ';
    memcpy (sym + (a - basep) + 1, b, l->p - b);
  }
  
  for (int i = 0; i < n; ++i)
    sym[i] = tolower (sym [i]);
  printf ("[lexer] symbol: %s\n", sym);
  return caos_value_symbol (sym);
}

int lex_integer_value (CaosLexer *l)
{
  char *basep = l->p;
  assert (isdigit (*l->p));
  while (isdigit (*l->p)) l->p++;
//  advance_while (l, (bool(*)(char))&isdigit);
  printf ("[lexer] int: %i\n", atoi(basep));
  return atoi (basep);
}

int* lex_bytestring_helper (CaosLexer *l, int i)
{
  int *a;
  int j;
  
  if (i > 255) abort();
  
  if (']' == *l->p) {
    a = calloc (sizeof(int), i+2); // TODO: Why is this +2?
    a[i] = -1;
  } else {
    j = lex_integer_value (l);
    skip_whitespace (l);
    a = lex_bytestring_helper (l, i+1);
    a[i] = j;
  }
  return a;
}

CaosValue lex_bytestring (CaosLexer *l)
{
  assert ('[' == *l->p++);
  int *b = lex_bytestring_helper (l, 0);
  assert (']' == *l->p++);
  printf ("[lexer] bytestring\n");
  return caos_value_bytestring (b);
}

CaosValue lex_albian_string (CaosLexer *l)
{
  assert ('[' == *l->p++);
  char *basep = l->p;
  advance_until(l, &isrightbracket);
  char *str = strndup (basep, l->p - basep);
  assert (']' == *l->p++);
  printf ("[lexer] string: %s\n", str);
  return caos_value_string (str);
}

CaosValue lex_double_quoted_string (CaosLexer *l)
{
  assert ('"' == *l->p++);
  char *basep = l->p;
  advance_until(l, &isdoublequote);
  // TODO: Escape codes!
  char *str = strndup (basep, l->p - basep);
  assert ('"' == *l->p++);
  printf ("[lexer] string: %s\n", str);
  return caos_value_string (str);
}

//
// THE BIG DADDY
// 
CaosValue caos_lexer_lex (CaosLexer *l)
{
  skip_whitespace_and_comments(l);

  if (!*l->p) return caos_value_eoi();
  
  if (isdigit (*l->p)) return lex_number (l);
  
  switch (*l->p)
  {
    case '+':
    case '-': return lex_number (l);
    case '%': return lex_integer_binary (l);
    case '\'': return lex_integer_character (l);
    case '=': l->p++; return caos_value_symbol ("eq");
    case '<': l->p++; switch (*l->p) {
      case '>': l->p++; return caos_value_symbol ("ne");
      case '=': l->p++; return caos_value_symbol ("le");
      default: return caos_value_symbol ("lt"); }
    case '>': switch (*l->p) {
      case '=': l->p++; return caos_value_symbol ("ge");
      default: return caos_value_symbol ("gt"); }
  }
  
  if (issymchar (*l->p)) return lex_symbol(l);
  
  if ('"' == *l->p) {
    assert (CAOS_EXODUS == l->version);
    return lex_double_quoted_string (l);
  }
  
  if ('[' == *l->p) {
    switch (l->version) {
    case CAOS_ALBIA:
      return lex_albian_string (l);
    case CAOS_EXODUS:
      return lex_bytestring (l);
    default:
      assert(false);
    }
  }
  
  printf ("Error '%c'\n", *l->p);
  abort();
  return caos_value_null();
}
