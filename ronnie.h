#ifndef RONNIE_H
#define RONNIE_H

#include "caos.h"

#include <stdint.h>

RONNIE_API bool  caos_arg_bool (CaosContext*);
RONNIE_API float caos_arg_float (CaosContext*);
RONNIE_API int   caos_arg_int (CaosContext*);
RONNIE_API char* caos_arg_string (CaosContext*);

RONNIE_API CaosValue caos_value_float (float);
RONNIE_API CaosValue caos_value_int (int);
RONNIE_API CaosValue caos_value_string (char*);

RONNIE_API bool caos_value_is_float (CaosValue);
RONNIE_API bool caos_value_is_integer (CaosValue);
RONNIE_API bool caos_value_is_string (CaosValue);

RONNIE_API float caos_value_to_float (CaosValue);
RONNIE_API int   caos_value_to_integer (CaosValue);
RONNIE_API char* caos_value_to_string (CaosValue);

RONNIE_API bool caos_value_equal (CaosValue, CaosValue);

// ~ Lexer ~

typedef struct CaosLexer {
  char *script, *p;
} CaosLexer;

RONNIE_API CaosLexer caos_lexer (char *script);
RONNIE_API CaosValue caos_lexer_lex (CaosLexer *lexer);

#endif // RONNIE_H
