#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "value.h"
#include "token.h"

//

static CaosToken *ip;

CaosToken next()
{
  return *ip++;
}

CaosValue pop_val ()
{
  return token_as_value (next());
}

CaosValue pop_val_of(ValueType wanted)
{
  CaosValue v = pop_val();

  if (wanted != VARIABLE && value_is_type (v, VARIABLE)) {
    v = variable_get(value_as_variable (v));
  }
  
  if (!value_is_type (v, wanted)) {
    printf ("[ERROR] Wrong type %i %i\n", wanted, value_get_type (v));
    abort();
  }
  return v;
}

int pop_int ()
{
  return value_as_integer(pop_val_of (INTEGER));
}

char* pop_string()
{
  return value_as_string(pop_val_of (STRING));
}

CaosVar pop_var()
{
  return value_as_variable(pop_val_of (VARIABLE));
}

void pop_stmt()
{
  CaosToken stmt = next();
  assert (token_is_type (stmt, TOKEN_FUNCTION));
  statement_call (token_as_function (stmt));
}

void caos_new_simp()
{
  int family = pop_int();
  int genus = pop_int();
  int species = pop_int();
  char *spritefile = pop_string();
  int image_count = pop_int();
  int first_image = pop_int();
  int plane = pop_int();

  printf ("new: simp %i %i %i \"%s\" %i %i %i\n",
    family, genus, species,
    spritefile, image_count, first_image, plane);
}

CaosValue caos_rand ()
{
  int left = pop_int();
  int right = pop_int();

  int r = rand() % (right - left) + left;
  
  return value_new_integer(r);
}

void caos_setv()
{
  CaosVar var = pop_var();
  CaosValue val = pop_val();

  variable_set (var, val);
}

void caos_dbg_outi()
{ 
  int i = pop_int();
  printf ("DBG: OUTI %i\n", i);
}

static CaosValue dummy_variable;

CaosValue caos_get_dummy_variable()
{
  return dummy_variable;
}

void caos_set_dummy_variable (CaosValue v)
{
  assert (value_is_type (v, INTEGER));
  printf ("Setting var to %i\n", value_as_integer (v));
  dummy_variable = v;
}

CaosValue caos_dummy_variable()
{
  // TODO: NULL CaosValue, and use a static variable here
  return value_new_variable (caos_get_dummy_variable, caos_set_dummy_variable);
}

int main()
{
  srand (time (NULL));

  CaosToken script[] = {
    token_new_function (caos_new_simp, NULL),
    token_new_integer (2),
    token_new_integer (4),
    token_new_integer (50000),
    token_new_string ("basicplant"),
    token_new_integer (3),
    token_new_integer (0),
    token_new_function (NULL, caos_rand),
    token_new_integer (200),
    token_new_integer (6000),
    token_new_function (caos_setv, NULL),
    token_new_function (NULL, caos_dummy_variable),
    token_new_integer (5),
    token_new_function (caos_dbg_outi, NULL),
    token_new_function (NULL, caos_dummy_variable)
  };
  ip = script;

  while (ip != script + sizeof(script) / sizeof(*script))
    pop_stmt();
}
