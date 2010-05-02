#ifndef CAOS_TOKEN_H
#define CAOS_TOKEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "type.h"

#include <stdbool.h>

typedef struct {
  CaosType type;
  union {
    int i;
    char *s;
  } value;
} CaosToken;


RONNIE_PUBLIC CaosToken token_int (int);
RONNIE_PUBLIC CaosToken token_string (char*);
RONNIE_PUBLIC CaosToken token_symbol (char*);
RONNIE_PUBLIC CaosToken token_eoi();
RONNIE_PUBLIC CaosToken token_null();

RONNIE_PUBLIC CaosType token_get_type (CaosToken);

RONNIE_PUBLIC bool token_is_symbol (CaosToken);

RONNIE_PUBLIC char* token_to_string (CaosToken);
RONNIE_PUBLIC int token_to_int (CaosToken);
RONNIE_PUBLIC char* token_to_symbol (CaosToken);

#ifdef __cplusplus
}
#endif

#endif // CAOS_TOKEN_H
