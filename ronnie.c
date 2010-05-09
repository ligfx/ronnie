#include "caos.h"
#include "ronnie.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
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

int*
caos_arg_bytestring (CaosContext *context)
{
  CaosValue next = caos_arg_value (context);
  if (!caos_value_is_bytestring (next)) {
    ERROR ("Expected byte-string");
    return NULL;
  }
  
  printf ("[debug] bytestring\n");
  return caos_value_to_bytestring (next);
}

// ~ extra value types ~

enum RonnieType {
  CAOS_STRING = 50,
  CAOS_INT,
  CAOS_FLOAT,
  CAOS_BYTESTRING
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

CaosValue
caos_value_bytestring (int *a)
{
  CaosValue val = { CAOS_BYTESTRING, (intptr_t)a };
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

bool
caos_value_is_bytestring (CaosValue val)
{
  return val.type == CAOS_BYTESTRING;
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

int*
caos_value_to_bytestring (CaosValue val)
{
  return (int*)val.value;
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

CaosLexer caos_lexer (enum CaosLexerVersion version, char *script) {
  CaosLexer lex = { script, script, version };
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

bool iseol (char c) {
  return c == '\n' || c == '\r';
}

bool isalbiastrchar (char c) {
  return c != ']';
}

bool isc2estrchar (char c) {
  return c != '"';
}

bool isbeginsymchar (char c) {
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
    return true;
  default:
    return false;
  }
}

bool issymchar (char c) {
  // $#a-zA-Z0-9:?!_+-
  if (isbeginsymchar (c)) return true;
  switch (c) {
    case '-': // really?
    case '+':
      return true;
  }
  return false;
}

void lex_whitespace (CaosLexer *l) {
  while (iswhitespace(*l->p)) l->p++;
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

char* lex_string_albia (CaosLexer *l)
{
  char *s = lex_string (l, 0, isalbiastrchar, NULL);
  l->p++; // skip endbrace
  return s;
}

char* lex_string_c2e (CaosLexer *l)
{
  char *s = lex_string (l, 0, isc2estrchar, NULL);
  // TODO: What about escaped characters?
  l->p++; // skip endquote
  return s;
}

char* lex_symbol (CaosLexer *l)
{
  return lex_string (l, 0, issymchar, (char(*)(char))tolower);
}

int lex_integer_character (CaosLexer *l) {
  int i = *l->p++;
  assert ('\'' == *l->p);
  l->p++;
  return i;
}

int lex_integer_binary (CaosLexer *l) {
  int i = 0;
  while ('0' == *l->p || '1' == *l->p) {
    i = (i << 1) + (*l->p++ == '1');
  }
  return i;
}

int lex_integer_literal (CaosLexer *l) {
  int i = *l->p++ - '0';
  while (isdigit(*l->p)) {
    i *= 10;
    i += *l->p++ - '0';
  }
  return i;
}

int lex_integer (CaosLexer *l) {
  if (isdigit (*l->p)) return lex_integer_literal(l);
  
  if ('%' == *l->p) { l->p++; return lex_integer_binary(l); }
  if ('\'' == *l->p) { l->p++; return lex_integer_character(l); }

  abort();
}

int* lex_bytestring (CaosLexer *l, int i) {
  int *a;
  int j;
  
  if (i > 1000) abort();
  
  if (']' == *l->p) {
    l->p++;
    a = calloc (sizeof(int), i+2);
    a[i] = -1;
  } else {
    j = lex_integer (l);
    lex_whitespace (l);
    a = lex_bytestring (l, i+1);
    a[i] = j;
  }
  return a;
}

CaosValue lex_value_numeral (CaosLexer *l) {
  int i = lex_integer_literal (l);
  if ('.' == *l->p) {
    float f = i;
    i = 1;
    
    l->p++;
    while (isdigit (*l->p)) {
      f += (*l->p++ - '0') / pow (10.0, i++);
    }
    return caos_value_float(f);
  }
  return caos_value_int(i);
}

CaosValue lex_value_negative_numeral (CaosLexer *l) {
  CaosValue v = lex_value_numeral (l);
  if (caos_value_is_integer (v))
    return caos_value_int (-caos_value_to_integer (v));
  if (caos_value_is_float (v))
    return caos_value_float (-caos_value_to_float (v));
  
  abort();
}

CaosValue caos_lexer_lex (CaosLexer *l) {
start:
  lex_whitespace(l);

  if (!*l->p) return caos_value_eoi();
  
  if ('*' == *l->p) { // comment
    while (!iseol (*l->p)) l->p++;
    goto start;
  }
  
  if (isdigit (*l->p)) // numeral
    return lex_value_numeral (l);
  
  if (isbeginsymchar (*l->p)) // symbol
    return caos_value_symbol (lex_symbol(l));
  
  if (CAOS_EXODUS == l->version)
    if ('"' == *l->p) { // c2e string
      l->p++;
      return caos_value_string (lex_string_c2e (l));
    }
  
  switch (*(l->p++))
  {
    case '+': // positive number
      return lex_value_numeral (l);
    case '-': // negative number
      return lex_value_negative_numeral (l);
    case '%': // binary number
      return caos_value_int (lex_integer_binary (l));
    case '\'': // character
      return caos_value_int (lex_integer_character (l));
    case '[': // albia string, c2e byte-string
      if (CAOS_ALBIA == l->version)
        return caos_value_string (lex_string_albia (l));
      else if (CAOS_EXODUS == l->version)
        return caos_value_bytestring (lex_bytestring (l, 0));
      else
        abort();
    case '=':
      return caos_value_symbol ("eq");
    case '<':
      switch (*l->p)
      {
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
  }
  
  l->p--;
  printf ("Error '%c'\n", *l->p);
  abort();
  return caos_value_null();
}

CaosValue* ronnie_script_from_string (enum CaosLexerVersion version, char *source) {
  int i, m;
  i = m = 0;
  CaosLexer lexer = caos_lexer (version, source);
  CaosValue *script = NULL;

  while (true) {
    CaosValue val = caos_lexer_lex(&lexer);
    if (i == m) {
      m = m ? m << 1 : 1;
      script = (CaosValue*) realloc (script, sizeof(CaosValue) * m);
      assert (script);
    }
    script[i++] = val;

    if (caos_value_is_eoi (val)) break;
  }
  
  return script;
}
