#ifndef CAOS_DAIRY_H
#define CAOS_DAIRY_H

#ifndef CAOS_H
#error Must include caos.h before dairy.h
#endif

#include <stdint.h>

typedef bool (*caos_comparison_t) (CaosValue, CaosValue);
typedef bool (*caos_logical_t) (bool, bool);

RONNIE_PUBLIC caos_comparison_t comparison_from_symbol (char *sym);

RONNIE_PUBLIC caos_logical_t logical_from_symbol (char*);

RONNIE_PUBLIC bool caos_arg_bool (CaosContext*);
RONNIE_PUBLIC int caos_arg_int (CaosContext*);
RONNIE_PUBLIC char* caos_arg_string (CaosContext*);

RONNIE_PUBLIC CaosValue caos_value_bool_new (bool);
RONNIE_PUBLIC bool caos_value_is_bool (CaosValue);
RONNIE_PUBLIC bool caos_value_as_bool (CaosValue);

#endif // CAOS_DAIRY_H
