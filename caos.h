#ifndef CAOS_H
#define CAOS_H

#include "common.h"
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CaosRuntime CaosRuntime;
typedef struct CaosContext CaosContext;

typedef void (*caos_command_t) (CaosContext*);
typedef CaosValue (*caos_expression_t) (CaosContext*);

typedef void (*caos_script_advance_t) (void*);
typedef CaosValue (*caos_script_get_t) (void*);
typedef void (*caos_script_jump_t) (void*, int);
typedef int (*caos_script_mark_t) (void*);

struct ICaosScript {
  caos_script_advance_t advance;
  caos_script_get_t get;
  caos_script_jump_t jump;
  caos_script_mark_t mark;
};

// ~ Runtime ~

RONNIE_API CaosRuntime* caos_runtime_new();
RONNIE_API void caos_runtime_destroy(CaosRuntime*);

RONNIE_API void caos_register_function
  (CaosRuntime*, char*, caos_command_t, caos_expression_t);

// ~ Context ~

RONNIE_API CaosContext* caos_context_new(CaosRuntime*, void*, struct ICaosScript);
RONNIE_API void caos_context_destroy(CaosContext*);
//RONNIE_API void caos_reset (CaosContext*, void*);

// main thread
RONNIE_API void caos_tick (CaosContext*, void *user_data);
RONNIE_API bool caos_done (CaosContext*);

// caos functions

// stack
RONNIE_API void caos_stack_push (CaosContext*, int);
RONNIE_API int caos_stack_pop (CaosContext*);
RONNIE_API int caos_stack_peek (CaosContext*);

// positions
RONNIE_API int caos_mark (CaosContext*);
RONNIE_API void caos_jump (CaosContext*, int);
RONNIE_API void caos_advance (CaosContext*);
RONNIE_API void caos_fast_forward (CaosContext*, ...);

// arguments
RONNIE_API CaosValue caos_arg_value (CaosContext*);
RONNIE_API char* caos_arg_symbol (CaosContext*);
RONNIE_API CaosValue caos_current_token (CaosContext*);

// exceptions
RONNIE_API char* caos_get_error (CaosContext*);
RONNIE_API void caos_set_error (CaosContext*, char*);

// user
RONNIE_API void* caos_user_data (CaosContext*);

RONNIE_API void* caos_get_script (CaosContext*);

#ifdef __cplusplus
}
#endif

#endif // CAOS_H
