#include "caos.h"
#include "caos_private.h"
#include "map.h"

#include <assert.h>
#include <string.h>

bool
labels_equal (char **left, char **right)
{
  return (strcmp (*left, *right) == 0);
}

CaosValue
caos_value_int_new (int i)
{
  CaosValue val = { CAOS_INT, .value.i = i };
  return val;
}

CaosValue
caos_value_string_new (char *s)
{
  CaosValue val = { CAOS_STRING, .value.s = s };
  return val;
}

CaosValue
caos_value_null()
{
  CaosValue val;
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

int
caos_value_as_integer (CaosValue val)
{
  return val.value.i;
}

char*
caos_value_as_string (CaosValue val)
{
  return val.value.s;
}

CaosRuntime*
caos_runtime_new() {
  CaosRuntime *runtime = malloc (sizeof (*runtime));
  map_init (
    &runtime->functions,
    sizeof (char*), sizeof (FunctionRef),
    (map_equals_t) &labels_equal
  );
  map_init (
    &runtime->binomial_functions,
    sizeof (char*), sizeof (Map),
    (map_equals_t) &labels_equal
  );

  return runtime;
}

void
caos_register_function (
  CaosRuntime *runtime,
  char *label,
  caos_command_t command,
  caos_expression_t expression
) {
  FunctionRef f = { command, expression };
  // TODO: Error if label already registered
  map_set (&runtime->functions, &label, &f);
}

void
caos_register_binomial_function (
  CaosRuntime *runtime,
  char *base,
  char *label,
  caos_command_t command,
  caos_expression_t expression
) {
  FunctionRef f = { command, expression };

  // TODO: Error if base already registered as function

  Map* functions = (Map*)map_get (&runtime->binomial_functions, &base);
  if (!functions)
  {
    // Woops, base hasn't been setup yet
    // Create a map, and try again
    Map m;
    map_init (&m, sizeof (char*), sizeof (FunctionRef), (map_equals_t) &labels_equal);
    map_set (&runtime->binomial_functions, &base, &m);
    caos_register_binomial_function (runtime, base, label, command, expression);
    return;
  }
  // TODO: Error if label already registered
  map_set (functions, &label, &f);
}

CaosContext*
caos_context_new (CaosRuntime *runtime) {
  CaosContext *context = malloc (sizeof (*context)); {
    context->runtime = runtime;

    context->error = NULL;
    
    context->script = NULL;
    context->ip = NULL;
  }
  return context;
}

void
caos_set_script (CaosContext *context, CaosToken script[])
{
  context->script = script;
  context->ip = script;
}

CaosToken
caos_next_token (CaosContext *context)
{
  return *context->ip++;
}

void
caos_set_error (CaosContext *context, char *message)
{
  if (!context->error)
    context->error = message;
}

char*
caos_get_error (CaosContext *context)
{
  return context->error;
}

FunctionRef
caos_next_binomial_function (CaosContext *context, char *base)
{
  static FunctionRef null_func = { NULL, NULL };

  Map *binomial = map_get (&context->runtime->binomial_functions, &base);
  if (!binomial) return null_func;

  CaosToken next = caos_next_token (context);
  if (token_get_type(next) != TOKEN_SYMBOL) return null_func;
  char *label = token_as_string (next);
  
  void *value = map_get (binomial, &label);
  if (!value) return null_func;

  return *(FunctionRef*) value;
}


FunctionRef
caos_lookup_function_with_label (CaosContext *context, char *label)
{
  void *value = map_get (&context->runtime->functions, &label);
  if (!value) return caos_next_binomial_function (context, label);

  return *(FunctionRef*) value;
}

FunctionRef
caos_next_function (CaosContext *context)
{
  static FunctionRef null_func = { NULL, NULL };

  CaosToken next = caos_next_token (context);
  if (token_get_type(next) != TOKEN_SYMBOL) return null_func;
  char *label = token_as_string (next);

  return caos_lookup_function_with_label (context, label);
}

CaosValue
caos_next_value (CaosContext *context)
{
  CaosToken token = caos_next_token (context);

  caos_expression_t expr;
  switch (token_get_type (token))
  {
    case TOKEN_INTEGER:
      return caos_value_int_new (token_as_int (token));
    case TOKEN_STRING:
      return caos_value_string_new (token_as_string (token));
    case TOKEN_SYMBOL:
      expr = caos_lookup_expression_with_label (context, token_as_string (token));
      if (expr)
        return expr (context);
      else
        caos_set_error (context, "No such expression");
    default:
      caos_set_error (context, "Cannot convert token into value");
      return caos_value_null();
  }
}

int
caos_next_int (CaosContext *context)
{
  CaosValue next = caos_next_value (context);
  if (!caos_value_is_integer (next)) {
    caos_set_error (context, "Expected integer");
    return -42;
  }

  return caos_value_as_integer (next);
}


char*
caos_next_string (CaosContext *context)
{
  CaosValue next = caos_next_value (context);
  if (!caos_value_is_string (next)) {
    caos_set_error (context, "Expected string");
    return "Emm fibble!";
  }

  return caos_value_as_string (next);
}

caos_command_t
caos_next_command (CaosContext *context)
{
  return caos_next_function (context).command;
}

caos_expression_t
caos_lookup_expression_with_label (CaosContext *context, char *label)
{
  return caos_lookup_function_with_label (context, label).expression;
}

caos_expression_t
caos_next_expression (CaosContext *context)
{
  return caos_next_function (context).expression;
}

void
caos_tick (CaosContext *context)
{
  caos_command_t command = caos_next_command (context);
  if (command) command (context);
  else caos_set_error (context, "Expected command");
}
