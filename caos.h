#ifndef CAOS_H
#define CAOS_H

#include "token.h"

typedef enum CaosType {
  CAOS_STRING,
  CAOS_INT,
} CaosType;

typedef struct CaosValue {
  CaosType type;
  union {
    int i;
    char *s;
  } value;
} CaosValue;

typedef struct CaosRuntime CaosRuntime;
typedef struct CaosContext CaosContext;

typedef void (*caos_command_t) (CaosContext*);
typedef CaosValue (*caos_expression_t) (CaosContext*);

CaosValue caos_value_int_new (int);
CaosValue caos_value_string_new (char*);
CaosValue caos_value_null ();

bool caos_value_is_integer (CaosValue);
bool caos_value_is_string (CaosValue);

int caos_value_as_integer (CaosValue);
char* caos_value_as_string (CaosValue);

CaosRuntime* caos_runtime_new();
void caos_register_function
       (CaosRuntime*, char*, caos_command_t, caos_expression_t);
void caos_register_binomial_function
       (CaosRuntime*, char*, char*, caos_command_t, caos_expression_t);

CaosContext* caos_context_new(CaosRuntime*);

int caos_next_int (CaosContext*);
char* caos_next_string (CaosContext*);

char* caos_get_error (CaosContext*);

void caos_set_script (CaosContext*, CaosToken[]);

void caos_tick (CaosContext*);

#endif // CAOS_H
