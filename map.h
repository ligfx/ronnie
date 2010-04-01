#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct MapNode MapNode;

typedef bool (*map_equals_t)(void*, void*);

typedef struct Map {
  MapNode *first;
  size_t key_size;
  size_t value_size;
  map_equals_t compare;
} Map;

void map_init (Map*, size_t key_size, size_t value_size, map_equals_t);

// TODO: map_add
// TODO: map iterate

void map_set (Map*, void *key, void *value);

void* map_get (Map*, void *key);

bool map_exists (Map*, void *key);

void map_destroy (Map*);

#endif // MAP_H
