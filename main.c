#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "caos.h"

bool c_and (CaosValue left, CaosValue right)
{
  assert (caos_value_is_bool (left));
  assert (caos_value_is_bool (right));
  return caos_value_as_bool (left) == caos_value_as_bool (right);
}

bool c_eq (CaosValue left, CaosValue right)
{
  return caos_value_equal (left, right);
}

void c_bam (CaosContext *context)
{
  printf ("BAM!!\n");
}

void c_outs (CaosContext *context)
{
  char *string = caos_arg_string (context);
  if (caos_get_error (context)) return;

  printf ("%s\n", string);
}

void c_new_simp (CaosContext *context)
{

  int family = caos_arg_int (context),
      genus = caos_arg_int (context),
      species = caos_arg_int (context);
  char* spritefile = caos_arg_string (context);
  int image_count = caos_arg_int (context),
      first_image = caos_arg_int (context),
      plane = caos_arg_int (context);
  if (caos_get_error (context)) return;

  printf ("new: simp %i %i %i \"%s\" %i %i %i\n",
    family, genus, species,
    spritefile, image_count, first_image, plane);
}

CaosValue c_rand (CaosContext *context)
{
  int left = caos_arg_int (context);
  int right = caos_arg_int (context);
  if (caos_get_error (context)) return caos_value_null();

  int result = rand() % (right - left) + left;
  return caos_value_int_new (result);
}

void
c_reps (CaosContext *context) {
  // Stack []
  int loops_left = caos_arg_int (context);
  if (caos_get_error (context)) return;
  
  caos_stack_push (context, caos_mark (context));
  caos_stack_push (context, loops_left);
  // Stack [pos, loops_left]
}

void
c_repe (CaosContext *context) {
  // Stack [pos, loops_left]
  int loops_left = caos_stack_pop (context) - 1;
  if (loops_left == 0) { // Done!
    int pos = caos_stack_pop (context);
    // Stack []
  } else {
    int pos = caos_stack_peek (context);
    caos_jump (context, pos);
    caos_stack_push (context, loops_left);
    // Stack [pos, loops_left]
  }
}

void
c_doif (CaosContext *context) {
  // Stack []
  bool match = caos_arg_bool (context);

  caos_stack_push (context, match);
  caos_stack_push (context, 0);
  if (!match)
    caos_fast_forward (context, "elif", "else", "endi", 0);
  // Stack [already_matched]
}

void
c_elif (CaosContext *context) {
  // Stack [already_matched]
  if (caos_stack_peek (context) == true // already matched
  // || caos_arg_bool (context) == false // didn't match this time
  ) {
    caos_fast_forward (context, "elif", "else", "endi", 0);
    return;
  }

  (void) caos_stack_pop (context);
  caos_stack_push (context, true);
  // Stack [already_matched]
}

void
c_else (CaosContext *context) {
  // Stack [already_matched]

  if (caos_stack_peek (context) == true) {
    caos_fast_forward (context, "endi", 0);
  }

  // Stack [already_matched]
}

void
c_endi (CaosContext *context) {
  // Stack [already_matched]

  (void) caos_stack_pop (context);

  // Stack []
}

int main ()
{
  char *error;
  srand (time (NULL));

  CaosToken script[] = {
    token_symbol_new ("reps"),
    token_int_new (5),
    token_symbol_new ("bam!"),
    token_symbol_new ("new:"),
    token_symbol_new ("simp"),
    token_int_new (2),
    token_int_new (4),
    token_int_new (50000),
    token_string_new ("basicplant"),
    token_int_new (3),
    token_int_new (0),
    token_symbol_new ("rand"),
    token_int_new (200),
    token_int_new (5000),
    token_symbol_new ("repe"),
    token_symbol_new ("bam!"),
    token_symbol_new ("doif"),
    token_int_new (0),
    token_symbol_new ("eq"),
    token_int_new (1),
    token_symbol_new ("outs"),
    token_string_new ("This should be skipped right now"),
    token_symbol_new ("endi"),
    token_symbol_new ("bam!"),
    token_eoi()
  };

  //

  CaosRuntime *runtime = caos_runtime_new();
  CaosContext *context = caos_context_new (runtime);
  
  caos_register_function (runtime, "bam!", c_bam, 0);
  caos_register_function (runtime, "doif", c_doif, 0);
  caos_register_function (runtime, "endi", c_endi, 0);
  caos_register_function (runtime, "outs", c_outs, 0);
  caos_register_function (runtime, "rand", 0, c_rand);
  caos_register_function (runtime, "repe", c_repe, 0);
  caos_register_function (runtime, "reps", c_reps, 0);
  caos_register_binomial_function (runtime, "new:", "simp", c_new_simp, 0);

  caos_register_condition (runtime, 0, "and", c_and);
//  caos_register_condition (runtime, 0, "or", c_or);
  caos_register_condition (runtime, 10, "eq", c_eq);
//  caos_register_condition (runtime, 10, "ne", c_ne);

  caos_set_script (context, script);

  while (!caos_done (context)) {
    caos_tick (context);
    if ((error = caos_get_error (context))) printf ("[ERROR] %s\n", error);
  }
}
