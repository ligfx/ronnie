#include "map.h"
#include "map-private.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void*
map_node_get_key (Map *map, MapNode *node)
{
  return ((void*)node) + sizeof(MapNode);
}

void
map_node_set_key (Map *map, MapNode *node, void *key)
{
  memcpy (map_node_get_key (map, node), key, map->key_size);
}

void*
map_node_get_value (Map *map, MapNode *node)
{
  return map_node_get_key (map, node) + map->key_size;
}

void
map_node_set_value (Map *map, MapNode *node, void *value)
{
  memcpy (map_node_get_value (map, node), value, map->value_size);
}

MapNode*
map_node_new (Map *map, void *key, void *value)
{
  MapNode *node = malloc (sizeof(MapNode) + map->key_size + map->value_size);
  map_node_set_key (map, node, key);
  map_node_set_value (map, node, value);
  node->next = NULL;
  return node;
}

void
map_node_destroy (Map *map, MapNode *node)
{
  if (node->next)
    map_node_destroy (map, node->next);
  free (node);
}

void
map_init (Map *map, size_t key_size, size_t value_size, map_equals_t compare)
{
  map->first = NULL;
  map->key_size = key_size;
  map->value_size = value_size;
  map->compare = compare;
}

void
map_set (Map *map, void *key, void *value)
{
  if (!map->first) {
    map->first = map_node_new (map, key, value);
    return;
 }

  MapNode *node = map->first;
  if (node) while (true) {
    if (map->compare (map_node_get_key (map, node), key)) {
      map_node_set_value (map, node, value);
      return;
    }
    if (!node->next) break;
  };
  // Key does not exist
  // 'node' should be the last Node in the stack

  node->next = map_node_new (map, key, value);
}

void*
map_get (Map *map, void *key)
{
  MapNode *node = map->first;
  while (node) {
    if (map->compare (map_node_get_key (map, node), key))
      return map_node_get_value (map, node);
    node = node->next;
  }
  return NULL;
}

bool
map_exists (Map *map, void *key)
{
  MapNode *node = map->first;
  while (node) {
    if (map->compare (map_node_get_key (map, node), key))
      return true;
    node = node->next;
  };
  return false;
}

void
map_destroy (Map *map)
{
  map_node_destroy (map, map->first);
}
