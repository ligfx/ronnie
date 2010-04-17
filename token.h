#ifndef CAOS_TOKEN_H
#define CAOS_TOKEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

#include <stdbool.h>

typedef struct {
  CaosType type;
  union {
    int i;
    char *s;
  } value;
} CaosToken;


CaosToken token_int_new (int);
CaosToken token_string_new (char*);
CaosToken token_symbol_new (char*);
CaosToken token_eoi();
CaosToken token_null();

CaosType token_get_type (CaosToken);
bool token_is_type (CaosToken, CaosType);

char* token_as_string (CaosToken);
int token_as_int (CaosToken);
char* token_as_symbol (CaosToken);

#ifdef __cplusplus
}
#endif

#endif // CAOS_TOKEN_H
