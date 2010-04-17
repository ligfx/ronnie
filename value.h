#ifndef CAOS_VALUE_H
#define CAOS_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

#include <stdbool.h>

typedef struct CaosValue {
  CaosType type;
  union {
    int i;
    char *s;
  } value;
} CaosValue;

CaosValue caos_value_int_new (int);
CaosValue caos_value_string_new (char*);
CaosValue caos_value_null ();

bool caos_value_is_integer (CaosValue);
bool caos_value_is_string (CaosValue);

int caos_value_as_integer (CaosValue);
char* caos_value_as_string (CaosValue);

#ifdef __cplusplus
}
#endif

#endif // CAOS_VALUE_H
