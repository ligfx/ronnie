/*
   Structs
*/

#include "token.h"

/*
   Constructors
*/


CaosToken token_int (int i)
{
  CaosToken t = { CAOS_INT, .value.i = i };
  return t;
}

CaosToken token_float (float f)
{
  CaosToken t = { CAOS_FLOAT, .value.f = f };
  return t;
}

CaosToken token_string (char *s)
{
  CaosToken t = { CAOS_STRING, .value.s = s };
  return t;
}

CaosToken token_symbol (char *s)
{
  CaosToken t = { CAOS_SYMBOL, .value.s = s };
  return t;
}

CaosToken token_eoi()
{
  CaosToken t = { CAOS_EOI };
  return t;
}

CaosToken token_null()
{
  static CaosToken t = { CAOS_NULL };
  return t;
}

/*
   Type-checking
*/

CaosType token_get_type (CaosToken t)
{
  return t.type;
}

bool token_is_symbol (CaosToken t)
{
  return t.type == CAOS_SYMBOL;
}

bool token_is_null (CaosToken t)
{
  return t.type == CAOS_NULL;
}

bool token_is_eoi (CaosToken t)
{
  return t.type == CAOS_EOI;
}

/*
   Conversions
*/

char* token_to_string (CaosToken t) {
  return t.value.s;
}

int token_to_int (CaosToken t) {
  return t.value.i;
}

float token_to_float (CaosToken t) {
  return t.value.f;
}

char* token_to_symbol (CaosToken t) {
  return t.value.s;
}

/*CaosValue token_as_value (CaosToken t) {
  switch (token_get_type (t)) {
  case TOKEN_INTEGER:  return value_new_integer (token_as_integer (t));
  case TOKEN_STRING:   return value_new_string (token_as_string (t));
  case TOKEN_FUNCTION: return expression_call (token_as_function (t));
  default:
    assert (NULL && "Not a valid token!");
  }
}*/
