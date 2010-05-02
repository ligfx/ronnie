#include "caos.h"
#include "ronnie.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR(msg) caos_set_error(context, (char*)msg)

// ~ arg_bool ~

// Comparisons

bool c_and (bool left, bool right)
{
  return left && right;
}

bool c_or (bool left, bool right)
{
  return left || right;
}

bool c_eq (CaosValue left, CaosValue right)
{
  return caos_value_equal (left, right);
}

bool c_ne (CaosValue left, CaosValue right)
{
  return !c_eq (left, right);
}

#ifndef streq
#define streq(left, right) (strcmp(left, right) == 0)
#endif

typedef bool (*caos_comparison_t) (CaosValue, CaosValue);
typedef bool (*caos_logical_t) (bool, bool);

caos_comparison_t
comparison_from_symbol (char *sym)
{
  if (streq (sym, "eq")) return c_eq;
  if (streq (sym, "ne")) return c_ne;

  return NULL;
}

caos_logical_t
logical_from_symbol (char *sym)
{
  if (streq (sym, "and")) return c_and;
  if (streq (sym, "or")) return c_or;

  return NULL;
}

bool
caos_arg_bool (CaosContext *context)
{

  CaosValue left, right;
  caos_comparison_t compare_func;
  caos_logical_t logic_func;
  bool ret, second;

  left = caos_arg_value (context);
  compare_func = comparison_from_symbol (caos_arg_symbol (context));
  right = caos_arg_value (context);
  if (caos_get_error (context)) return false;
  if (!compare_func) {
    caos_set_error (context, (char*)"No such comparison");
    return false;
  }
  
  ret = compare_func (left, right);

  while (true) {
    {
      if (caos_done (context)) break;
      CaosValue next = caos_current_token (context);
      if (!caos_value_is_symbol (next)) break;

      logic_func = logical_from_symbol (caos_value_to_symbol (next));
      if (!logic_func) break;
      caos_advance (context); // Because current_token does not advance!
    }

    left = caos_arg_value (context);
    compare_func = comparison_from_symbol (caos_arg_symbol (context));
    right = caos_arg_value (context);
    if (caos_get_error (context)) return false;
    if (!compare_func) {
      caos_set_error (context, (char*)"No such comparison");
      return false;
    }

    second = compare_func (left, right);
    ret = logic_func (ret, second);
  }

  return ret;
}

// ~ typed arg functions ~

float
caos_arg_float (CaosContext *context)
{
  CaosValue next = caos_arg_value (context);
  if (!caos_value_is_float (next)) {
    ERROR ("Expected float");
    return -42;
  }

  printf ("[DEBUG] Float '%f'\n", caos_value_to_float (next));
  return caos_value_to_float (next);
}

int
caos_arg_int (CaosContext *context)
{
  CaosValue next = caos_arg_value (context);
  if (!caos_value_is_integer (next)) {
    ERROR ("Expected integer");
    return -42;
  }

  printf ("[DEBUG] Int '%i'\n", caos_value_to_integer (next));
  return caos_value_to_integer (next);
}

char*
caos_arg_string (CaosContext *context)
{
  CaosValue next = caos_arg_value (context);
  if (!caos_value_is_string (next)) {
    ERROR ("Expected string");
    return NULL;
  }

  printf ("[DEBUG] String '%s'\n", caos_value_to_string (next));
  return caos_value_to_string (next);
}

// ~ extra value types ~

enum DairyType {
  CAOS_STRING = 50,
  CAOS_INT,
  CAOS_FLOAT,
};

CaosValue
caos_value_int (int i)
{
  CaosValue val = { CAOS_INT, i };
  return val;
}

CaosValue
caos_value_string (char *s)
{
  CaosValue val = { CAOS_STRING, (intptr_t)s };
  return val;
}

CaosValue
caos_value_float (float f)
{
  CaosValue val;
  val.type = CAOS_FLOAT;
  val.value = *(intptr_t*)&f;
  return val;
}

bool
caos_value_is_integer (CaosValue val)
{
  return val.type == CAOS_INT;
}

bool
caos_value_is_string (CaosValue val)
{
  return val.type == CAOS_STRING;
}

bool
caos_value_is_float (CaosValue val)
{
  return val.type == CAOS_FLOAT;
}

int
caos_value_to_integer (CaosValue val)
{
  return val.value;
}

char*
caos_value_to_string (CaosValue val)
{
  return (char*)val.value;
}

float
caos_value_to_float (CaosValue val)
{
  return *(float*)&val.value;
}

// ~ Value helper functions ~

bool
caos_value_equal (CaosValue left, CaosValue right)
{
  // TODO: Really, everything should just be numbers
  //   But we're using string pointers because it's easier right now
  if (caos_value_is_string (left) && caos_value_is_string (right)) {
    return strcmp (caos_value_to_string (left), caos_value_to_string (right)) == 0;
  }
  if (left.type != right.type) {
    return false;
  }
  return left.value == right.value;
}

//
// ~ Lexer ~
//

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

bool isstrchar (char c) {
  return c != '"';
}

bool issymchar (char c) {
  // $#a-zA-Z0-9:?!_+-
  if (isalnum(c)) return true;
  switch (c) {
  case '$':
  case '#':
  case ':':
  case '?':
  case '!':
  case '_':
  case '+':
  case '-': // really?
    return true;
  default:
    return false;
  }
}

char* lex_string (CaosLexer *l, int i, bool (*keep_going) (char), char (*trans)(char)) {
  char *s;
  char c = *l->p;
  if (trans) c = trans (c);

  if (i > 1000) abort();
  
  if (!keep_going(c))
    s = calloc (1, i + 1);
  else {
    l->p++;
    s = lex_string (l, i + 1, keep_going, trans);
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
    return caos_value_symbol (lex_string (l, 0, issymchar, (char(*)(char))tolower));
  }

  char *s;
  switch (c) {
  case '"':
    s = lex_string (l, 0, isstrchar, NULL);
    l->p++; // skip endquote
    return caos_value_string (s);
  case '=':
    return caos_value_symbol ("eq");
  case '<':
    switch (*l->p) {
    case '>':
      l->p++;
      return caos_value_symbol ("ne");
    case '=':
      l->p++;
      return caos_value_symbol ("le");
    default:
      return caos_value_symbol ("lt");
    }
  case '>':
    if (*l->p == '=') {
      l->p++;
      return caos_value_symbol ("ge");
    } else {
      return caos_value_symbol ("gt");
    }
  default:
    printf ("Error! '%c'\n", c);
    abort();
    return caos_value_null();
  }
}
