#ifndef CAOS_TOKEN_H
#define CAOS_TOKEN_H

/*
   Structs
*/

#include <stdbool.h>

//#include "value.h"

typedef enum {
    TOKEN_INTEGER,
    TOKEN_STRING,
    TOKEN_SYMBOL
} TokenType;

typedef struct {
  TokenType type;
  union {
    int i;
    char *s;
  } value;
} CaosToken;


CaosToken token_int_new (int);
CaosToken token_string_new (char*);
CaosToken token_symbol_new (char*);

TokenType token_get_type (CaosToken);

char* token_as_string (CaosToken);
int token_as_int (CaosToken);
char* token_as_symbol (CaosToken);

// CaosValue token_as_value (CaosToken);

#endif // CAOS_TOKEN_H
