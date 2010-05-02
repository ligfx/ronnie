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
  CaosToken *script;
  CaosToken *ip;
};

void script_advance (struct Script *script) {
  script->ip++;
}

CaosToken script_get (struct Script *script) {
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

  CaosToken script[] = {
    token_symbol ("reps"),
    token_int (5),
    token_symbol ("bam!"),
    token_symbol ("new:"),
    token_symbol ("simp"),
    token_int (2),
    token_int (4),
    token_int (50000),
    token_string ("basicplant"),
    token_int (3),
    token_int (0),
    token_symbol ("rand"),
    token_int (200),
    token_int (5000),
    token_symbol ("repe"),
    token_symbol ("bam!"),
    token_symbol ("doif"),
    token_int (0),
    token_symbol ("eq"),
    token_int (1),
    token_symbol ("outs"),
    token_string ("This should be skipped right now"),
    token_symbol ("endi"),
    token_symbol ("doif"),
    token_int (5),
    token_symbol ("eq"),
    token_int (5),
    token_symbol ("and"),
    token_int (1),
    token_symbol ("ne"),
    token_int (2),
    token_symbol ("or"),
    token_int (6),
    token_symbol ("eq"),
    token_int (7),
    token_symbol ("outs"),
    token_string ("This, on the other hand, should be output"),
    token_symbol ("endi"),
    token_symbol ("bam!"),
    token_symbol ("outs"),
    token_string ("Test OUTS"),
    token_symbol ("doif"),
    token_int (0),
    token_symbol ("eq"),
    token_int (1),
    token_symbol ("endi"),
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
