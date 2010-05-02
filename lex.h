#ifndef CAOS_LEXER_H
#define CAOS_LEXER_H

#include "common.h"
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CaosLexer {
  char *script, *p;
} CaosLexer;

RONNIE_PUBLIC CaosLexer caos_lexer (char *script);
RONNIE_PUBLIC CaosValue caos_lexer_lex (CaosLexer *lexer);

#ifdef __cplusplus
}
#endif

#endif // CAOS_H
