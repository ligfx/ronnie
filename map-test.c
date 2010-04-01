#include "map.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// What I want to do

bool string_equal (char **left, char **right) {
  return strcmp (*left, *right) == 0;
}

int main () {

  Map m;
  map_init (&m, sizeof(char*), sizeof(char*), (map_equals_t)string_equal);

  char *k = "test";
  char *v = "blam";
  map_set (&m, &k, &v);

  assert (strcmp (*(char**)map_get (&m, &k), v) == 0); 

  assert (map_exists (&m, &k) == true);
  k = "nukl";
  assert (map_exists (&m, &k) == false);
  assert (map_get (&m, &k) == NULL);

  k = "test";
  printf ("%s\n", *(char**)map_get (&m, &k));

  k = "name";
  v = "Jim";
  map_set (&m, &k, &v);

  printf ("Name: %s\n", *(char**)map_get (&m, &k));

  k = "another";
  map_set (&m, &k, &v);

  map_destroy (&m);
}
