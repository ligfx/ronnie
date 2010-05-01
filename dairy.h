#ifndef CAOS_DAIRY_H
#define CAOS_DAIRY_H

#ifndef CAOS_H
#error Must include caos.h before dairy.h
#endif

typedef bool (*caos_comparison_t) (CaosValue, CaosValue);
typedef bool (*caos_logical_t) (bool, bool);

caos_comparison_t comparison_from_symbol (char *sym);

caos_logical_t logical_from_symbol (char*);

bool caos_arg_bool (CaosContext*);
int caos_arg_int (CaosContext*);
char* caos_arg_string (CaosContext*);
char* caos_arg_symbol (CaosContext*);

CaosValue caos_value_float_new (float);
CaosValue caos_value_bool_new (bool);
bool caos_values_is_float (CaosValue);
bool caos_value_is_bool (CaosValue);
float caos_value_as_float (CaosValue);
bool caos_value_as_bool (CaosValue);

#endif // CAOS_DAIRY_H
