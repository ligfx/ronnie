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

CaosRuntime* caos_runtime_new();
void caos_register_function
       (CaosRuntime*, char*, caos_command_t, caos_expression_t);
void caos_register_binomial_function
       (CaosRuntime*, char*, char*, caos_command_t, caos_expression_t);

CaosContext* caos_context_new(CaosRuntime*);

CaosToken caos_fast_forward (CaosContext*, ...);

void caos_stack_push (CaosContext*, int);
int caos_stack_pop (CaosContext*);
int caos_stack_peek (CaosContext*);

int caos_mark (CaosContext*);
void caos_jump (CaosContext*, int);

bool caos_done (CaosContext*);

CaosValue caos_arg_value (CaosContext*);
int caos_arg_int (CaosContext*);
char* caos_arg_string (CaosContext*);

char* caos_get_error (CaosContext*);

void caos_set_script (CaosContext*, CaosToken[]);

void caos_tick (CaosContext*);

#ifdef __cplusplus
}
#endif

#endif // CAOS_H
