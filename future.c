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
