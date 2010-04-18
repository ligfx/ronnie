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

void c_new_simp (CaosContext *context)
{

  int family = caos_arg_int (context),
      genus = caos_arg_int (context),
      species = caos_arg_int (context);
  char *spritefile = caos_arg_string (context);
  int image_count = caos_arg_int (context),
      first_image = caos_arg_int (context),
      plane = caos_arg_int (context);
  if (caos_get_error (context)) return;

  printf ("new: simp %i %i %i \"%s\" %i %i %i\n",
    family, genus, species,
    spritefile, image_count, first_image, plane);
}

runtime.RegisterBinomialFunction ("new:", "simp", context => {
  var next = context.Next;
  int family = next().AsInt(),
      genus = next().AsInt(),
      species = next().AsInt();
  string spritefile = next().AsString();
  int image_count = next().AsInt(),
      first_image = next().AsInt(),
      plane = next().AsInt();
  
}, Null);

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

bool
caos_next_condition (CaosContext *context) {
  // Shunting yard

  ValueStack stack;
  IntStack ops;

  valuestack_push (stack, caos_next_value (context));

  do {
    CaosToken tok = caos_next_symbol (context);
    if (caos_get_error (context)) return false;
    
    while (!ops.empty() && streq (token_as_string(tok), "and")) {
      CaosValue right = stack.top(); stack.pop();
      CaosValue left = stack.top(); stack.pop();

      int op = ops.top(); ops.pop();
      CaosValue result;
      switch (op) {
        case CAOS_CONDITION_AND:
          assert (caos_value_is_bool (left));
          assert (caos_value_is_bool (right));
          result = caos_value_bool_new
            (caos_value_as_bool (left) == caos_value_as_bool (right));
          break;
        case CAOS_CONDITION_EQ:
          result = caos_value_bool_new (caos_value_equal (left, right));
          break;
        default: abort();
      }
      valuestack_push (stack, new);
    }
    intstack_push (ops, tok);

    valuestack_push (caos_next_value (context));
    if (caos_get_error (context)) return false;
  } while (token_is_symbol(caos_peek_token(context)));

  while (!intstack_empty (ops)) {
    CaosValue left = valuestack_pop (stack);
    CaosValue right = valuestack_pop (stack);

    int op = intstack_pop (ops);
    CaosValue result;
    switch (op) {
      case CAOS_CONDITION_AND:
        assert (caos_value_is_bool (left));
        assert (caos_value_is_bool (right));
        result = caos_value_bool_new
          (caos_value_as_bool (left) == caos_value_as_bool (right));
        break;
      case CAOS_CONDITION_EQ:
        result = caos_value_bool_new (caos_value_equal (left, right));
        break;
      default: abort();
    }
    valuestack_push (stack, new);
  }

  assert (caos_value_is_bool (valuestack_peek (stack)));
  return caos_value_bool_new (valuestack_pop (stack));
}
