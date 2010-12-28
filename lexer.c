#include "ronnie.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// ~ Lexer ~
//

CaosLexer caos_lexer (enum CaosLexerVersion version, const char *script) {
  CaosLexer lex = { script, (char*)script, 1, version };
  return lex;
}

CaosLexError*
caos_lex_error_new (CaosLexErrorType type, unsigned int lineno, void *data) {
  CaosLexError *e = malloc (sizeof (CaosLexError));
  e->type = type;
  e->lineno = lineno;
  e->data = data;

  return e;
}

void
caos_lex_error_free (CaosLexError *err)
{
  free (err);
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

bool isnewline (char c) {
  switch (c) {
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
  while (func (*l->p)) {
    if (isnewline (*l->p)) l->lineno++;
    l->p++;
  }
}

void advance_until (CaosLexer *l, bool (*func)(char))
{
  while (!func (*l->p) && (*l->p)) {
    if (isnewline (*l->p)) l->lineno++;
    l->p++;
  }
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

CaosValue lex_number (CaosLexer *l, CaosLexError **e) {
  char *basep = l->p;
  
  CaosLexErrorType op = CAOS_UNKNOWN;
  if ('+' == *l->p) {
    l->p++;
    op = CAOS_MISLEADING_UNARY_PLUS;
  } else if ('-' == *l->p) {
    l->p++;
    op = CAOS_MISLEADING_UNARY_MINUS;
  }
  // assert (isdigit (*l->p));
  if (!isdigit (*l->p)) {
    *e = caos_lex_error_new (op, l->lineno, NULL);
    return caos_value_null();
  }
  
  while (isdigit (*l->p)) l->p++;
//  advance_while (l, (bool(*)(char))&isdigit);
  if ('.' == *l->p) {
    l->p++;
    while (isdigit (*l->p)) l->p++;
//    advance_while (l, (bool(*)(char))&isdigit);
  
    //printf ("[lexer] float: %f\n", atof(basep));
    return caos_value_float (atof (basep));
  } else {
    //printf ("[lexer] int: %i\n", atoi(basep));
    return caos_value_int (atoi (basep));
  }
}

CaosValue lex_integer_character (CaosLexer *l, CaosLexError **e) {
  assert ('\'' == *l->p++);
  int i = *l->p++;
  
  if ('\'' != (*l->p++)) {
    *e = caos_lex_error_new (CAOS_MISLEADING_SINGLE_QUOTE, l->lineno, NULL);
    return caos_value_null();
  }
  
  //printf ("[lexer] int: %i\n", i);
  return caos_value_int(i);
}

CaosValue lex_integer_binary (CaosLexer *l) {
  int i = 0;
  assert ('%' == *l->p++);
  while ('0' == *l->p || '1' == *l->p) {
    i = (i << 1) + (*l->p++ == '1');
  }
  //printf ("[lexer] int: %i\n", i);
  return caos_value_int(i);
}

int lex_integer_value (CaosLexer *l)
{
  char *basep = l->p;
  assert (isdigit (*l->p));
  while (isdigit (*l->p)) l->p++;
//  advance_while (l, (bool(*)(char))&isdigit);
  // printf ("[lexer] int: %i\n", atoi(basep));
  return atoi (basep);
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
  //printf ("[lexer] symbol: %s\n", sym);
  return caos_value_symbol (sym);
}

int* lex_bytestring_helper (CaosLexer *l, CaosLexError **e, int i)
{
  int *a;
  int j;
  
  if (i > 255) abort();
  
  if (']' == *l->p) {
    a = calloc (sizeof(int), i+2); // TODO: Why is this +2?
    a[i] = -1;
  } else {
    if (!isdigit (*l->p)) {
      if ('\0' == *l->p)
        *e = caos_lex_error_new (CAOS_UNCLOSED_BYTESTRING, l->lineno, NULL);
      else
        *e = caos_lex_error_new (CAOS_BYTESTRING_EXPECTED_INTEGER, l->lineno, NULL);
      return NULL;
    }
    
    j = lex_integer_value (l);
    skip_whitespace (l);
    a = lex_bytestring_helper (l, e, i+1);
    if (!a) return NULL;
    a[i] = j;
  }
  return a;
}

CaosValue lex_bytestring (CaosLexer *l, CaosLexError **e)
{
  assert ('[' == *l->p++);
  //printf ("[lexer] bytestring\n");
  
  int *b = lex_bytestring_helper (l, e, 0);
  if (!b) return caos_value_null();
  
  assert (']' == *l->p++);
  return caos_value_bytestring (b);
}

CaosValue lex_albian_string (CaosLexer *l, CaosLexError **e)
{
  assert ('[' == *l->p++);
  char *basep = l->p;
  advance_until(l, &isrightbracket);

  if ('\0' == *l->p) {
    *e = caos_lex_error_new (CAOS_UNCLOSED_STRING, l->lineno, NULL);
    return caos_value_null();
  }
  
  char *str = strndup (basep, l->p - basep);
  assert (']' == *l->p++);
  //printf ("[lexer] string: %s\n", str);
  return caos_value_string (str);
}

CaosValue lex_exodus_string (CaosLexer *l, CaosLexError **e)
{
  assert ('"' == *l->p++);
  char *basep = l->p;
  advance_until(l, &isdoublequote);

  if ('\0' == *l->p) {
    *e = caos_lex_error_new (CAOS_UNCLOSED_STRING, l->lineno, NULL);
    return caos_value_null();
  }
  
  // TODO: Escape codes!
  char *str = strndup (basep, l->p - basep);
  assert ('"' == *l->p++);
  //printf ("[lexer] string: %s\n", str);
  return caos_value_string (str);
}

//
// THE BIG DADDY
// 
CaosValue the_big_daddy (CaosLexer *l, CaosLexError **e)
{
	skip_whitespace_and_comments(l);
	
	if (!*l->p) return caos_value_eoi();
	
	if (isdigit (*l->p)) return lex_number (l, e);
	
	switch (*l->p)
	{
		case '+':
		case '-': return lex_number (l, e);
		case '%': return lex_integer_binary (l);
		case '\'': return lex_integer_character (l, e);
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
		return lex_exodus_string (l, e);
	}
	
	if ('[' == *l->p) {
		switch (l->version) {
			case CAOS_ALBIA:
				return lex_albian_string (l, e);
			case CAOS_EXODUS:
				return lex_bytestring (l, e);
			default:
				assert(false);
		}
	}
	
	*e = caos_lex_error_new (CAOS_UNRECOGNIZED_CHARACTER, l->lineno, l->p);
	return caos_value_null();
	
}

CaosValue caos_lexer_lex (CaosLexer *l, CaosLexError **e)
{
	// This is a small shim on top of The Big Daddy (c)
	// because it's easier to add character information this way
	
	char *old_p;
	old_p = l->p;
	CaosValue val = the_big_daddy (l, e);
	if (*e) return val;
	
	while (iswhitespace (*old_p)) ++old_p;
	val.location = old_p - l->script;
	val.extent = l->p - old_p;
	val.line = l->lineno;
	
	return val;
}
