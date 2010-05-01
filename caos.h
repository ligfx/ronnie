#ifndef CAOS_H
#define CAOS_H

#include "token.h"
#include "type.h"
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CaosRuntime CaosRuntime;
typedef struct CaosContext CaosContext;

typedef void (*caos_command_t) (CaosContext*);
typedef CaosValue (*caos_expression_t) (CaosContext*);
typedef bool (*caos_condition_t) (CaosValue, CaosValue);

typedef void (*caos_script_advance_t) (void*);
typedef CaosToken (*caos_script_get_t) (void*);
typedef void (*caos_script_jump_t) (void*, int);
typedef int (*caos_script_mark_t) (void*);

struct ICaosScript {
  caos_script_advance_t advance;
  caos_script_get_t get;
  caos_script_jump_t jump;
  caos_script_mark_t mark;
};

CaosRuntime* caos_runtime_new();

void caos_register_function
  (CaosRuntime*, char*, caos_command_t, caos_expression_t);
void caos_register_binomial_function
  (CaosRuntime*, char*, char*, caos_command_t, caos_expression_t);
void caos_register_condition
  (CaosRuntime*, int precedence, char*, caos_condition_t);      

CaosContext* caos_context_new(CaosRuntime*);

CaosToken caos_fast_forward (CaosContext*, ...);

void caos_stack_push (CaosContext*, int);
int caos_stack_pop (CaosContext*);
int caos_stack_peek (CaosContext*);

int caos_mark (CaosContext*);
void caos_jump (CaosContext*, int);

bool caos_done (CaosContext*);

char* caos_arg_symbol (CaosContext*);
CaosValue caos_arg_value (CaosContext*);
int caos_arg_int (CaosContext*);
char* caos_arg_string (CaosContext*);
bool caos_arg_bool (CaosContext*);

char* caos_get_error (CaosContext*);

CaosRuntime* caos_get_runtime (CaosContext*);

void caos_set_script (CaosContext*, void*, struct ICaosScript);

void caos_tick (CaosContext*, void *user_data);
void* caos_user_data (CaosContext*);

#ifdef __cplusplus
}
#endif

#endif // CAOS_H
