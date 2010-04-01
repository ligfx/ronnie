#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "caos.h"

void c_bam (CaosContext *context)
{
  printf ("BAM!!\n");
}

void c_new_simp (CaosContext *context)
{

  int family = caos_next_int (context);
  int genus = caos_next_int (context);
  int species = caos_next_int (context);
  char *spritefile = caos_next_string (context);
  int image_count = caos_next_int (context);
  int first_image = caos_next_int (context);
  int plane = caos_next_int (context);
  if (caos_get_error (context)) return;

  printf ("new: simp %i %i %i \"%s\" %i %i %i\n",
    family, genus, species,
    spritefile, image_count, first_image, plane);
}

CaosValue c_rand (CaosContext *context)
{
  int left = caos_next_int (context);
  int right = caos_next_int (context);
  if (caos_get_error (context)) return caos_value_null();

  int result = rand() % (right - left) + left;
  return caos_value_int_new (result);
}

int main ()
{
  char *error;

  CaosToken script[] = {
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
  };

  CaosRuntime *runtime = caos_runtime_new();
  CaosContext *context = caos_context_new (runtime);

  srand (time (NULL));
  caos_register_function (runtime, "bam!", c_bam, 0);
  caos_register_function (runtime, "rand", 0, c_rand);

  caos_register_binomial_function (runtime, "new:", "simp", c_new_simp, 0);

  caos_set_script (context, script);

  caos_tick (context);
  if ((error = caos_get_error (context))) printf ("[ERROR] %s\n", error);

  caos_tick (context);
  if ((error = caos_get_error (context))) printf ("[ERROR] %s\n", error);
}
