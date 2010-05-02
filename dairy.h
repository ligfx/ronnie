#ifndef CAOS_DAIRY_H
#define CAOS_DAIRY_H

#ifndef CAOS_H
#error Must include caos.h before dairy.h
#endif

#include <stdint.h>

RONNIE_PUBLIC bool  caos_arg_bool (CaosContext*);
RONNIE_PUBLIC float caos_arg_float (CaosContext*);
RONNIE_PUBLIC int   caos_arg_int (CaosContext*);
RONNIE_PUBLIC char* caos_arg_string (CaosContext*);

RONNIE_PUBLIC CaosValue caos_value_float (float);
RONNIE_PUBLIC CaosValue caos_value_int (int);
RONNIE_PUBLIC CaosValue caos_value_string (char*);

RONNIE_PUBLIC bool caos_value_is_float (CaosValue);
RONNIE_PUBLIC bool caos_value_is_integer (CaosValue);
RONNIE_PUBLIC bool caos_value_is_string (CaosValue);

RONNIE_PUBLIC float caos_value_to_float (CaosValue);
RONNIE_PUBLIC int   caos_value_to_integer (CaosValue);
RONNIE_PUBLIC char* caos_value_to_string (CaosValue);

RONNIE_PUBLIC bool caos_value_equal (CaosValue, CaosValue);

// ~ Lexer ~

typedef struct CaosLexer {
  char *script, *p;
} CaosLexer;

RONNIE_PUBLIC CaosLexer caos_lexer (char *script);
RONNIE_PUBLIC CaosValue caos_lexer_lex (CaosLexer *lexer);

#endif // CAOS_DAIRY_H
