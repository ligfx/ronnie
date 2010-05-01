#include "caos.h"
#include "caos_private.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <list>

#define ERROR(msg) caos_set_error(context, (char*)msg)

CaosRuntime*
caos_runtime_new() {
  CaosRuntime *runtime = (CaosRuntime*) malloc (sizeof (*runtime)); {
    runtime->functions = std::map <char*, FunctionRef>();
    runtime->binomials = std::map <char*, std::map <char*, FunctionRef> >();
    runtime->conditions_precedence = std::map <char*, int>();
    runtime->conditions = std::map <char*, caos_condition_t>();
  }

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
  runtime->functions [label] = f;
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
  // TODO: Error if label already registered
  runtime->binomials [base] [label] = f;
}

void
caos_register_condition (
  CaosRuntime *runtime,
  int precedence,
  char *label,
  caos_condition_t func
) {

  // TODO: Error if label already registered as condition
  runtime->conditions_precedence [label] = precedence;
  runtime->conditions [label] = func;
}

CaosContext*
caos_context_new (CaosRuntime *runtime) {
  CaosContext *context = (CaosContext*) malloc (sizeof (*context)); {
    context->runtime = runtime;

    context->error = NULL;
    
    context->script = NULL;

    context->stack = new std::stack <int>();
  }
  return context;
}

void
caos_set_script (CaosContext *context, void *script, ICaosScript iface)
{
  context->script = script;
  context->script_iface = iface;
}

int
caos_mark (CaosContext *context)
{
  return context->script_iface.mark (context->script);
}

void
caos_jump (CaosContext *context, int mark)
{
  context->script_iface.jump (context->script, mark);
}

void
caos_stack_push (CaosContext *context, int i)
{
  context->stack->push (i);
}

int
caos_stack_pop (CaosContext *context)
{
  int i = caos_stack_peek (context);
  context->stack->pop();
  return i;
}

int
caos_stack_peek (CaosContext *context)
{
  return context->stack->top();
}

CaosToken
caos_get_token (CaosContext *context)
{
  if (caos_done (context)) {
    ERROR ("Expected token, got EOI");
    return token_null();
  }
  return context->script_iface.get (context->script);
}

void
caos_set_error (CaosContext *context, char *message)
{
  if (!context->error) context->error = message;
}

char*
caos_get_error (CaosContext *context)
{
  return context->error;
}

bool
caos_done (CaosContext *context)
{
  return context->error ||
        (context->script_iface.get (context->script).type == CAOS_EOI);
}

void caos_advance_to_next_symbol (CaosContext *context)
{
  CaosToken tok;
  do {
    caos_advance (context);
    tok = caos_get_token (context);
    if (caos_get_error (context)) return;
  } while (token_get_type (tok) != CAOS_SYMBOL);
}

CaosToken
caos_fast_forward (CaosContext *context, ...)
{
  std::list <char*> strings;

  {
    va_list args;
    char *s;
    va_start (args, context);
    while ((s = va_arg (args, char*)))
      strings.push_back (s);
    va_end (args);
  }

  while (true) {
    caos_advance_to_next_symbol(context);
    CaosToken sym = caos_get_token(context);
    if (caos_get_error (context)) return token_null();
    char *str = token_as_string (sym);

    std::list<char*>::iterator it, end;
    for (it = strings.begin(), end = strings.end(); it != end; ++it)
      if (strcmp (str, *it) == 0)
      {
        return sym;
      }
  }
}

CaosValue
caos_arg_value (CaosContext *context)
{
  CaosValue ret = caos_value_null();
  caos_expression_t expr = NULL;

  CaosToken token = caos_get_token (context);
  if (caos_get_error (context)) return ret;

  switch (token_get_type (token))
  {
    case CAOS_INT:
      ret = caos_value_int_new (token_as_int (token));
      break;
    case CAOS_STRING:
      ret = caos_value_string_new (token_as_string (token));
      break;
    case CAOS_SYMBOL:
      expr = caos_get_expression (context);
      if (expr)
        ret = expr (context);
      else {
        printf ("%s\n", token_as_string (token));
        ERROR ("No such expression");
      }
      break;
    default:
      ERROR ("Cannot convert token into value");
  }

  // caos_get_expression () advanced already
  if (!expr) caos_advance(context);
  return ret;
}

bool
caos_arg_bool (CaosContext *context)
{
  CaosRuntime *rt = context->runtime;

  // Shunting yard

  std::stack <CaosValue> stack;
  std::stack <char*> ops;

  stack.push (caos_arg_value (context));

  while (true)
  {
    char *sym = caos_arg_symbol (context);
    if (caos_get_error (context)) return false;

    printf ("Condition: %s\n", sym);
    if (!rt->conditions_precedence.count (sym)) {
      ERROR ("No such condition");
      return false;
    }
    int precedence = rt->conditions_precedence [sym];
    
    while (!ops.empty() && rt->conditions_precedence [ops.top()] <= precedence) {
      CaosValue right = stack.top(); stack.pop();
      CaosValue left = stack.top(); stack.pop();

      char *op = ops.top(); ops.pop();
      stack.push (caos_value_bool_new (rt->conditions [op] (left, right)));
    }
    ops.push (sym);

    stack.push (caos_arg_value (context));
    if (caos_get_error (context)) return false;

    // while
    {
      CaosToken next = caos_get_token (context);
      if (!token_is_symbol (next)) break;
      if (!rt->conditions_precedence.count (token_as_symbol (next))) break;
    }
  }

  while (!ops.empty()) {
    CaosValue right = stack.top(); stack.pop();
    CaosValue left = stack.top(); stack.pop();

    char *op = ops.top(); ops.pop();
    stack.push (caos_value_bool_new (rt->conditions [op] (left, right)));
  }

  assert (stack.size() == 1);
  assert (caos_value_is_bool (stack.top()));
  return caos_value_as_bool (stack.top());
}

int
caos_arg_int (CaosContext *context)
{
  CaosValue next = caos_arg_value (context);
  if (!caos_value_is_integer (next)) {
    ERROR ("Expected integer");
    return -42;
  }

  printf ("[DEBUG] Int '%i'\n", caos_value_as_integer (next));
  return caos_value_as_integer (next);
}

char*
caos_arg_string (CaosContext *context)
{
  CaosValue next = caos_arg_value (context);
  if (!caos_value_is_string (next)) {
    ERROR ("Expected string");
    return NULL;
  }

  printf ("[DEBUG] String '%s'\n", caos_value_as_string (next));
  return caos_value_as_string (next);
}

char*
caos_arg_symbol (CaosContext *context)
{
  CaosToken tok = caos_get_token (context);
  caos_advance (context);
  if (!token_is_symbol (tok)) return NULL;
  return token_as_string (tok);
}

FunctionRef
caos_get_function (CaosContext *context)
{
  static FunctionRef null_func = { NULL, NULL };
  CaosToken tok;
  char *label;

  label = caos_arg_symbol (context);
  if (!label) return null_func;
  printf ("[DEBUG] Function '%s'\n", label);

  std::map<char*, FunctionRef> &functions
    = context->runtime->functions;
  if (functions.count (label))
  {
    return functions[label];
  }

  std::map<char*, std::map<char*, FunctionRef> > &binomials
    = context->runtime->binomials;
  if (binomials.count (label))
  {
      std::map<char*, FunctionRef> &second
        = binomials [label];

      label = caos_arg_symbol (context);
      if (!label) return null_func;
      printf ("[DEBUG] Secondary Function '%s'\n", label);

      if (second.count (label))
        return second [label];
  }
  
  return null_func;
}

caos_command_t
caos_get_command (CaosContext *context)
{
  printf ("[DEBUG] Command\n");
  return caos_get_function (context).command;
}

caos_expression_t
caos_get_expression (CaosContext *context)
{
  return caos_get_function (context).expression;
}

void
caos_advance (CaosContext *context)
{
  context->script_iface.advance (context->script);
}

void
caos_tick (CaosContext *context)
{
  caos_command_t command = caos_get_command (context);
  if (command) command (context);
  else ERROR ("Expected command");
}
