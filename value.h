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
    bool b;
  } value;
} CaosValue;

CaosValue caos_value_int_new (int);
CaosValue caos_value_string_new (char*);
CaosValue caos_value_bool_new (bool);
CaosValue caos_value_null ();

bool caos_value_is_integer (CaosValue);
bool caos_value_is_string (CaosValue);
bool caos_value_is_bool (CaosValue);

int caos_value_as_integer (CaosValue);
char* caos_value_as_string (CaosValue);
bool caos_value_as_bool (CaosValue);

bool caos_value_equal (CaosValue, CaosValue);

#ifdef __cplusplus
}
#endif

#endif // CAOS_VALUE_H
