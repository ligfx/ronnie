#include "value.h"

#include <stdio.h>

CaosValue
caos_value_int_new (int i)
{
  CaosValue val = { CAOS_INT, .value.i = i };
  return val;
}

CaosValue
caos_value_string_new (char *s)
{
  CaosValue val = { CAOS_STRING, .value.s = s };
  return val;
}

CaosValue
caos_value_bool_new (bool b)
{
  CaosValue val = { CAOS_BOOL, .value.b = b };
  return val;
}

CaosValue
caos_value_null()
{
  CaosValue val;
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
caos_value_is_bool (CaosValue val)
{
  return val.type == CAOS_BOOL;
}

int
caos_value_as_integer (CaosValue val)
{
  return val.value.i;
}

char*
caos_value_as_string (CaosValue val)
{
  return val.value.s;
}

bool
caos_value_as_bool (CaosValue val)
{
  return val.value.b;
}

bool
caos_value_equal (CaosValue left, CaosValue right)
{
  if (caos_value_is_integer (left) && caos_value_is_integer (right)) {
    return caos_value_as_integer (left) == caos_value_as_integer (right);
  }
  printf ("[WARNING] Fudging equality test for non-integers\n");
  return false;
}
