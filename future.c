CaosRuntime *runtime;
CaosContext *context;

runtime = caos_runtime_new();
context = caos_context_new(runtime);

caos_add_function (runtime, NULL, c_rand, "rand");

caos_add_secondary_function (runtime, c_dbg_outi, NULL, "dbg:", "outi");
caos_add_secondary_function (runtime, c_new_simp, NULL, "new:", "simp");

// ...

caos_context_destroy (context);
caos_runtime_destroy (runtime);

void
c_dbg_outi (CaosContext *context) {
  int x = caos_get_integer (context);
  if (caos_error (context)) return;
  
  printf (
    "[DBG: OUTI @ %s:%i] %i\n",
    caos_script_name(context),
    caos_line_number(context),
    x);
}

CaosValue
c_rand (CaosContext *context) {
  int left = caos_get_integer (context);
  int right = caos_get_integer (context);
  if (caos_error (context)) return;

  int result = rand() % (right - left) + left;
  return caos_value_from_integer (result);
}

void
c_reps (CaosContext *context) {
  // Stack []
  int loops_left = caos_next_integer (context);
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
    caos_jump (pos);
    caos_stack_push (loops_left);
    // Stack [pos, loops_left]
  }
}

void
c_doif (CaosContext *context) {
  // Stack []
  bool match = caos_next_condition (context);

  caos_stack_push (match);
  if (!match)
    caos_jump_to_next_symbol_matching ("elif", "else", "endi", 0);
  // Stack [already_matched]
}

void
c_elif (CaosContext *context) {
  // Stack [already_matched]
  if (caos_stack_peek (context) == true // already matched
   || caos_next_condition (context) == false // didn't match this time
  ) {
    caos_jump_to_next_symbol_matching ("elif", "else", "endi", 0);
    return;
  }

  (void)caos_stack_pop (context);
  caos_stack_push (context, true);
  // Stack [already_matched]
}

void
c_else (CaosContext *context) {
  // Stack [already_matched]

  if (caos_stack_peek (context) == true) {
    caos_jump_to_next_symbol_matching ("endi", 0);
  }

  // Stack [already_matched]
}

void
c_endi (CaosContext *context) {
  // Stack [already_matched]

  caos_stack_pop (context);

  // Stack []
}
