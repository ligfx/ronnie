#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ronnie/ronnie.h>

/*

lsp: defn "bam!"
  dbg: outs "BAM!!"
lsp: endf

*/

void c_bam (CaosContext *context)
{
  printf ("BAM!!\n");
}

void c_outa (CaosContext *context)
{
  int *array = caos_arg_bytestring (context);
  if (caos_get_error (context)) return;
  
  int *i = array;
  while (*i >= 0) {
    printf ("%i ", *i++);
  }
  printf ("\n");
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

/*
  cfunction "rand" type:float type:void

  expression rand
    let left arg-int
    let right arg-int
  begin
    
    // (+ left (% (rand) (- right left)))
    return add get left mod cfunction "rand" sub get right get left
  ende
*/

CaosValue c_rand (CaosContext *context)
{
  int left = caos_arg_int (context);
  int right = caos_arg_int (context);
  if (caos_get_error (context)) return caos_value_null();

  int result = rand() % (right - left) + left;
  return caos_value_int (result);
}

/*

  command reps
    let loops arg-int
  begin

    push mark
    push loops
  endc

*/

void
c_reps (CaosContext *context) {
  // Stack []
  int loops_left = caos_arg_int (context);
  if (caos_get_error (context)) return;
  
  caos_stack_push (context, caos_mark (context));
  caos_stack_push (context, loops_left);
  // Stack [pos, loops_left]
}

/*

  command repe
  begin
    let loops sub pop 1

    doif get loops eq 0
      null pop
    else
      let pos = peek
      jump pos
      push loop
    endi  
  endc

*/

void
c_repe (CaosContext *context) {
  // Stack [pos, loops_left]
  int loops_left = caos_stack_pop (context) - 1;
  if (loops_left == 0) { // Done!
    (void) caos_stack_pop (context);
    // Stack []
  } else {
    int pos = caos_stack_peek (context);
    caos_jump (context, pos);
    caos_stack_push (context, loops_left);
    // Stack [pos, loops_left]
  }
}

/*
  command doif
    let match arg-bool
  begin
    
    push get match
    push 0
    doif get match eq false
      fast-forward "elif" "else" "endi" null
    endi
  endc
*/

void
c_doif (CaosContext *context) {
  // Stack []
  bool match = caos_arg_bool (context);
  if (caos_get_error (context)) return;

  caos_stack_push (context, match);
  // caos_stack_push (context, 0);
  if (!match)
    caos_fast_forward (context, "elif", "else", "endi", 0);
  // Stack [already_matched]
}

/*
  
  command elif
  begin
    
    doif peek eq true
      fast-forward "elif" "else" "endi" null
    else
      null pop
      push true
    end
  endc
  
*/

void
c_elif (CaosContext *context) {
  // Stack [already_matched]
  if (caos_stack_peek (context) == true) // already matched)
  {
    caos_fast_forward (context, "endi", 0);
    return;
  }
  
  bool match = caos_arg_bool (context);
  if (caos_get_error (context)) return;
  if (!match)// didn't match this time
  {
    caos_fast_forward (context, "elif", "else", "endi", 0);
    return;
  }

  (void) caos_stack_pop (context);
  caos_stack_push (context, true);
  // Stack [already_matched]
}

/*

  command else
    doif peek eq true
      fast-forward "endi" null
    endi
  endc

*/

void
c_else (CaosContext *context) {
  // Stack [already_matched]

  if (caos_stack_peek (context) == true) {
    caos_fast_forward (context, "endi", 0);
  }

  // Stack [already_matched]
}

/*
  command endi null pop endc
*/

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

  CaosValue script[] = {
    caos_value_symbol ("reps"),
    caos_value_int (5),
    caos_value_symbol ("bam!"),
    caos_value_symbol ("new: simp"),
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
    caos_value_string ("This, on the other hand, _should_ definitely be printed"),
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
  
  caos_register_function (runtime, "bam!", c_bam, 0);
  caos_register_function (runtime, "doif", c_doif, 0);
  caos_register_function (runtime, "endi", c_endi, 0);
  caos_register_function (runtime, "outa", c_outa, 0);
  caos_register_function (runtime, "outs", c_outs, 0);
  caos_register_function (runtime, "outv", c_outv, 0);
  caos_register_function (runtime, "rand", 0, c_rand);
  caos_register_function (runtime, "repe", c_repe, 0);
  caos_register_function (runtime, "reps", c_reps, 0);
  caos_register_function (runtime, "new: simp", c_new_simp, 0);

  {
    CaosContext *context = ronnie_context_new (runtime, script);

    while (!caos_done (context)) {
      caos_tick (context, NULL);
      if ((error = caos_get_error (context))) printf ("[ERROR] %s\n", error);
    }
  }

  { 
    /* CaosValue *script = ronnie_script_from_string (
      CAOS_ALBIA,
      "new: simp 2 4 5 [flower] 4 3 56 outs [hi]"); */

    CaosLexError *error = NULL;
    CaosValue *script = ronnie_script_from_string (
      CAOS_EXODUS,
      &error,
      "nEW: Simp 2 5 6 \"flower\" 4 3 rand 500 6000 doif 0 = 0 outs \"hello, world!\" bam! outv 42 endi * ignore me hahahadf45342frc23\n outv -5 outv 4.76 outv 'A' outv %00000011 outa [7 8]");
    
    if (error) {
      printf ("[Error] line %i, error %i\n", error->lineno, error->type);
      caos_lex_error_free (error);
    }
    else
    {
      CaosContext *context = ronnie_context_new (runtime, script);

      while (!caos_done (context)) {
        caos_tick (context, NULL);
        char *error = caos_get_error (context);
        if (error) printf ("[ERROR] %s\n", error);
      }
    }
  }

/*
  
  {
    CaosValue *script = ronnie_script_from_string (CAOS_EXODUS, "notacommand");
    CaosContext *context = ronnie_context_new (runtime, script);
    
    // Expected command, got Symbol:notacommand, at line 1
    caos_tick (context, NULL);
    printf ("%s\n", caos_get_error(context));
  }
  
  {
    CaosValue *script = ronnie_script_from_string (CAOS_EXODUS, "outv hi");
    CaosContext *context = ronnie_context_new (runtime, script);
    
    // Expected decimal, got Symbol:hi, at line 1, at Command:outv
    // Expected expression
    // Expected decimal expression
    // TODO: Not very easy to explain this..
    // Just learn what the error messages mean!
    caos_tick (context, NULL);
    printf ("%s\n", caos_get_error(context));
  }
  
  {
    CaosValue *script = ronnie_script_from_string (CAOS_EXODUS, "outv \"phi\"");
    CaosContext *context = ronnie_context_new (runtime, script);
    
    // Expected decimal expression, got String:"phi", at line 1, at Command:outv
    caos_tick (context, NULL);
    printf ("%s\n", caos_get_error(context));
  }
  
  {
    CaosValue *script = ronnie_script_from_string (CAOS_EXODUS, "outs rand 5 6");
    CaosContext *context = ronnie_context_new (runtime, script);
    
    // Expected string expression, got Integer:5, at line 1, at Command:outs
    // TODO: Could we do static analysis? Have each function register it's
    //  expected arguments and return value, then run over it to make sure?
    //  We could then differentiate between load-time analysis and run-time
    //  analysis. Expense: an extra pass. Benefit: knowing problems ahead of time
    //  Actually, all functions already register their arguments, they just
    //  don't register their return types. Expense: explicitly calling out return
    //  types of expressions. Are any expressions, say, bimorphic in their return
    //  types? I don't think so.. They should return either one type or any type.
    //  But I'm not solid on this.
    caos_tick (context, NULL);
    printf ("%s\n", caos_get_error(context));
  }
  */
}
