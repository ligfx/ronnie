#ifndef CAOS_PRIVATE_H
#define CAOS_PRIVATE_H

#include "caos.h"

#include <map>
#include <stack>


typedef struct FunctionRef {
  caos_command_t command;
  caos_expression_t expression;
} FunctionRef;

struct CaosRuntime {
  std::map <char*, FunctionRef> functions;
  std::map <char*, std::map <char*, FunctionRef> > binomials;

  std::map <char*, int> conditions_precedence;
  std::map <char*, caos_condition_t> conditions;
};

struct CaosContext {
  CaosRuntime *runtime;
  char *error;
  CaosToken *script;
  CaosToken *ip;
  std::stack<int> *stack;
};

CaosRuntime* caos_get_runtime (CaosContext*);

CaosToken caos_get_token (CaosContext*);

void caos_advance (CaosContext*);
void caos_advance_to_next_symbol (CaosContext*);

void caos_set_error (CaosContext*, char*);

FunctionRef caos_get_function (CaosContext*);
caos_command_t caos_get_command (CaosContext*);
caos_expression_t caos_get_expression (CaosContext*);

#endif // CAOS_PRIVATE_H
