#ifndef CAOS_PRIVATE_H
#define CAOS_PRIVATE_H

#include "caos.h"

#include <map>
#include <stack>
#include <string>


typedef struct FunctionRef {
  caos_command_t command;
  caos_expression_t expression;
} FunctionRef;

struct CaosRuntime {
  std::map <std::string, FunctionRef> functions;
};

struct CaosContext {
  CaosRuntime *runtime;
  char *error;
  void *script;
  ICaosScript script_iface;
  void *user_data;
  std::stack<int> *stack;
};

void caos_advance_to_next_symbol (CaosContext*);
void caos_override_error (CaosContext*, char*);

FunctionRef caos_get_function (CaosContext*);
caos_command_t caos_get_command (CaosContext*);
caos_expression_t caos_get_expression (CaosContext*);

#endif // CAOS_PRIVATE_H
