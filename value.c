#include "value.h"

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
