#include "value.h"

#include <stdio.h>
#include <string.h>

CaosValue
caos_value_int_new (int i)
{
  CaosValue val = { CAOS_INT, i };
  return val;
}

CaosValue
caos_value_string_new (char *s)
{
  CaosValue val = { CAOS_STRING, (intptr_t)s };
  return val;
}

CaosValue
caos_value_float_new (float f)
{
  CaosValue val;
  val.type = CAOS_FLOAT;
  val.value = *(intptr_t*)&f;
  return val;
}

CaosValue
caos_value_null()
{
  CaosValue val;
  val.type = CAOS_NULL;
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
caos_value_is_null (CaosValue val)
{
  return val.type == CAOS_NULL;
}

int
caos_value_as_integer (CaosValue val)
{
  return val.value;
}

char*
caos_value_as_string (CaosValue val)
{
  return (char*)val.value;
}

float
caos_value_as_float (CaosValue val)
{
  return *(float*)&val.value;
}

bool
caos_value_equal (CaosValue left, CaosValue right)
{
  // TODO: Really, everything should just be numbers
  //   But we're using string pointers because it's easier right now
  if (caos_value_is_string (left) && caos_value_is_string (right)) {
    return strcmp (caos_value_as_string (left), caos_value_as_string (right)) == 0;
  }
  if (left.type != right.type) {
    return false;
  }
  return left.value == right.value;
}
