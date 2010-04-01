#ifndef MAP_PRIVATE_H
#define MAP_PRIVATE_H

#include "map.h"

struct MapNode {
  MapNode *next;
};

void* map_node_get_key (Map*, MapNode*);

void map_node_set_key (Map*, MapNode*, void *key);

void* map_node_get_value (Map*, MapNode*);

void map_node_set_value (Map*, MapNode*, void *value);

MapNode* map_node_new (Map*, void *key, void *value);

void map_node_destroy (Map*, MapNode*);

#endif // MAP_PRIVATE_H
