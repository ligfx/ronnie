#ifndef CAOS_PRIVATE_H
#define CAOS_PRIVATE_H

#include "caos.h"
#include "map.h"
#include "intstack.h"

typedef struct FunctionRef {
  caos_command_t command;
  caos_expression_t expression;
} FunctionRef;

struct CaosRuntime {
  Map functions;
  Map binomial_functions;
};

struct CaosContext {
  CaosRuntime *runtime;
  char *error;
  CaosToken *script;
  CaosToken *ip;
  IntStack stack;
};

CaosRuntime* caos_get_runtime (CaosContext*);

CaosToken caos_next_token (CaosContext*);

void caos_set_error (CaosContext*, char*);

FunctionRef caos_next_binomial_function (CaosContext*, char *base);

FunctionRef caos_lookup_function_with_label (CaosContext*, char *label);
FunctionRef caos_next_function (CaosContext*);

void caos_rewind_once (CaosContext*);
CaosToken caos_jump_to_next_symbol (CaosContext*);

caos_command_t caos_next_command (CaosContext*);

caos_expression_t caos_lookup_expression_with_label (CaosContext*, char *label);
caos_expression_t caos_next_expression (CaosContext*);

#endif // CAOS_PRIVATE_H
