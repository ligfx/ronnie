#ifndef RONNIE_H
#define RONNIE_H

#include "caos.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

RONNIE_API bool  caos_arg_bool (CaosContext*);
RONNIE_API float caos_arg_float (CaosContext*);
RONNIE_API int   caos_arg_int (CaosContext*);
RONNIE_API char* caos_arg_string (CaosContext*);
RONNIE_API int*  caos_arg_bytestring (CaosContext*);

RONNIE_API CaosValue caos_value_float (float);
RONNIE_API CaosValue caos_value_int (int);
RONNIE_API CaosValue caos_value_string (char*);
RONNIE_API CaosValue caos_value_bytestring (int*);

RONNIE_API bool caos_value_is_float (CaosValue);
RONNIE_API bool caos_value_is_integer (CaosValue);
RONNIE_API bool caos_value_is_string (CaosValue);
RONNIE_API bool caos_value_is_bytestring (CaosValue);

RONNIE_API float caos_value_to_float (CaosValue);
RONNIE_API int   caos_value_to_integer (CaosValue);
RONNIE_API char* caos_value_to_string (CaosValue);
RONNIE_API int*  caos_value_to_bytestring (CaosValue);

RONNIE_API bool caos_value_equal (CaosValue, CaosValue);

// ~ Lexer ~

enum CaosLexerVersion {
  CAOS_ALBIA,
  CAOS_EXODUS
};

typedef struct CaosLexer {
  char *script, *p;
  unsigned int lineno;
  enum CaosLexerVersion version;
} CaosLexer;

typedef enum {
  CAOS_UNKNOWN = 0,
  CAOS_MISLEADING_UNARY_PLUS,
  CAOS_MISLEADING_UNARY_MINUS,
  CAOS_UNCLOSED_STRING,
  CAOS_UNCLOSED_BYTESTRING,
  CAOS_MISLEADING_SINGLE_QUOTE,
  CAOS_UNRECOGNIZED_CHARACTER,
  CAOS_BYTESTRING_EXPECTED_INTEGER,
} CaosLexErrorType;

typedef struct CaosLexError {
    CaosLexErrorType type;
    unsigned int lineno;
    void *data;
} CaosLexError;

RONNIE_API CaosLexer caos_lexer (enum CaosLexerVersion, char *script);
RONNIE_API CaosValue caos_lexer_lex (CaosLexer *lexer, CaosLexError**);
RONNIE_API void caos_lex_error_free (CaosLexError*);

// ~ Helpers ~
RONNIE_API CaosScript* caos_script_from_string (enum CaosLexerVersion, CaosLexError**, char*);

#ifdef __cplusplus
}
#endif

#endif // RONNIE_H
