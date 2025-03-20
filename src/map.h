#ifndef nol_map_h
#define nol_map_h

#include "common.h"

// The map only support value of int that is 0 or more.
// Value of -1 means key does not exist,
// Value of -2 means tombstone.

typedef struct {
  const char* key;
  int32_t value;
} Entry;

typedef struct {
  int count;
  int capacity;
  Entry* entries;
} Map;

void init_map(Map* map);
void free_map(Map* map);

void map_set(Map* map, const char* key, int32_t value);
void map_add_all(Map* to, Map* from);
int32_t map_get(Map* map, const char* key);
void map_delete(Map* map, const char* key);

#endif