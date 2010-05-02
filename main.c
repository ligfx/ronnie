#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "caos.h"
#include "dairy.h"

void c_bam (CaosContext *context)
{
  printf ("BAM!!\n");
}

void c_outs (CaosContext *context)
{ 
  char *string = caos_arg_string (context);
  if (caos_get_error (context)) return;

  // smith_cost (caos_user_data (context), 1);

  printf ("%s\n", string);
}

void c_outv (CaosContext *context)
{
  CaosValue v = caos_arg_value (context);
  if (caos_value_is_integer (v))
    printf ("%i\n", caos_value_to_integer (v));
  else if (caos_value_is_float (v))
    printf ("%f\n", caos_value_to_float (v));
  else
    caos_set_error (context, (char*)"Expected decimal");
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
  return caos_value_int (result);
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
  if (caos_get_error (context)) return;

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

struct Script {
  CaosValue *script;
  CaosValue *ip;
};

void script_advance (struct Script *script) {
  script->ip++;
}

CaosValue script_get (struct Script *script) {
  return *script->ip;
}


void script_jump (struct Script *script, int mark) {
  script->ip = script->script + mark;
}

int script_mark (struct Script *script) {
  return script->ip - script->script;
}

int main ()
{
  char *error;
  srand (time (NULL));

  CaosValue script[] = {
    caos_value_symbol ("reps"),
    caos_value_int (5),
    caos_value_symbol ("bam!"),
    caos_value_symbol ("new:"),
    caos_value_symbol ("simp"),
    caos_value_int (2),
    caos_value_int (4),
    caos_value_int (50000),
    caos_value_string ("basicplant"),
    caos_value_int (3),
    caos_value_int (0),
    caos_value_symbol ("rand"),
    caos_value_int (200),
    caos_value_int (5000),
    caos_value_symbol ("repe"),
    caos_value_symbol ("bam!"),
    caos_value_symbol ("doif"),
    caos_value_int (0),
    caos_value_symbol ("eq"),
    caos_value_int (1),
    caos_value_symbol ("outs"),
    caos_value_string ("This should be skipped right now"),
    caos_value_symbol ("endi"),
    caos_value_symbol ("doif"),
    caos_value_int (5),
    caos_value_symbol ("eq"),
    caos_value_int (5),
    caos_value_symbol ("and"),
    caos_value_int (1),
    caos_value_symbol ("ne"),
    caos_value_int (2),
    caos_value_symbol ("or"),
    caos_value_int (6),
    caos_value_symbol ("eq"),
    caos_value_int (7),
    caos_value_symbol ("outs"),
    caos_value_string ("This, on the other hand, should be output"),
    caos_value_symbol ("endi"),
    caos_value_symbol ("bam!"),
    caos_value_symbol ("outs"),
    caos_value_string ("Test OUTS"),
    caos_value_symbol ("doif"),
    caos_value_int (0),
    caos_value_symbol ("eq"),
    caos_value_int (1),
    caos_value_symbol ("endi"),
    caos_value_symbol ("outv"),
    caos_value_float (5.4),
    caos_value_eoi()
  };

  //

  CaosRuntime *runtime = caos_runtime_new();
  
  CaosContext *context = caos_context_new (runtime);
  
  caos_register_function (runtime, "bam!", c_bam, 0);
  caos_register_function (runtime, "doif", c_doif, 0);
  caos_register_function (runtime, "endi", c_endi, 0);
  caos_register_function (runtime, "outs", c_outs, 0);
  caos_register_function (runtime, "outv", c_outv, 0);
  caos_register_function (runtime, "rand", 0, c_rand);
  caos_register_function (runtime, "repe", c_repe, 0);
  caos_register_function (runtime, "reps", c_reps, 0);
  caos_register_binomial_function (runtime, "new:", "simp", c_new_simp, 0);

  struct Script s = { script, script };
  struct ICaosScript i = {
    (caos_script_advance_t) script_advance, 
    (caos_script_get_t)     script_get,
    (caos_script_jump_t)    script_jump,
    (caos_script_mark_t)    script_mark
  };

  caos_set_script (context, &s, i);

  while (!caos_done (context)) {
    caos_tick (context, NULL);
    if ((error = caos_get_error (context))) printf ("[ERROR] %s\n", error);
  }
}
