#ifndef CAOS_H
#define CAOS_H

#include "common.h"
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CaosRuntime CaosRuntime;
typedef struct CaosContext CaosContext;
typedef struct CaosScript CaosScript;
typedef struct CaosError CaosError;

typedef void (*caos_command_t) (CaosContext*);
typedef CaosValue (*caos_expression_t) (CaosContext*);

// ~ Script ~

RONNIE_API CaosScript* caos_script_from_array (CaosValue*);

// ~ Runtime ~

RONNIE_API CaosRuntime* caos_runtime_new();
RONNIE_API void caos_runtime_destroy(CaosRuntime*);

RONNIE_API void caos_register_function
  (CaosRuntime*, char*, caos_command_t, caos_expression_t);

// ~ Context ~

RONNIE_API CaosContext* caos_context_new(CaosRuntime*, CaosScript*);
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
typedef int CaosErrorType;
enum {
	CAOS_NULL_ERROR = 0,
	CAOS_EXPECTED_COMMAND,
	CAOS_EXPECTED_EXPRESSION,
	CAOS_FAILED_TO_FAST_FORWARD,
};
	
struct CaosError {
	CaosErrorType type;
	CaosValue token;
};
	
RONNIE_API CaosError* caos_get_error (CaosContext*);
RONNIE_API void caos_set_error (CaosContext*, CaosErrorType, CaosValue);
RONNIE_API void caos_clear_error (CaosContext*);

// user
/* RONNIE_API CaosScript* caos_get_script (CaosContext*); */
RONNIE_API void* caos_user_data (CaosContext*);

#ifdef __cplusplus
}
#endif

#endif // CAOS_H
