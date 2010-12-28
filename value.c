#include "value.h"

#include <stdio.h>
#include <string.h>

CaosValue
caos_value_symbol (char *s)
{
  CaosValue val = { CAOS_SYMBOL, (intptr_t)s };
  return val;
}

CaosValue
caos_value_eoi ()
{
  CaosValue val = { CAOS_EOI };
  return val;
}

CaosValue
caos_value_null()
{
	CaosValue val = { CAOS_NULL };
	return val;
}

bool caos_value_is_eoi (CaosValue val) { return val.type == CAOS_EOI; }
bool caos_value_is_symbol (CaosValue val) { return val.type == CAOS_SYMBOL; }
bool caos_value_is_null (CaosValue val) { return val.type == CAOS_NULL; }

char* caos_value_to_symbol (CaosValue val) { return (char*)val.value; }
