#include "map.h"

#include "memory.h"

#define TABLE_MAX_LOAD 0.75

void init_map(Map* map) {
  map->count = 0;
  map->capacity = 0;
  map->entries = NULL;
}

void free_map(Map* map) {
  FREE_ARRAY(Entry, map->entries, map->capacity);
  initTable(map);
}

uint32_t hash_string(const char* key) {
  uint32_t hash = 2166136261u;

  int i = 0;
  while (key[i] != '\0') {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;

    i++;
  }

  return hash;
}

Entry* find_entry(Entry* entries, int capacity, const char* key) {
  uint32_t index = hash_string(key) % capacity;
  Entry* tombstone = NULL;

  while (true) {
    Entry* entry = &entries[index];

    if (entry->key == NULL) {
      if (entry->value == -1) {
        // Empty entry.
        return tombstone != NULL ? tombstone : entry;
      } else {
        // We found a tombstone.
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (entry->key == key) {
      // We found the key.
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

void adjust_capacity(Map* map, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);

  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = -1;
  }

  map->count = 0;

  for (int i = 0; i < map->capacity; i++) {
    Entry* entry = &map->entries[i];
    if (entry->key == NULL) continue;

    Entry* dest = find_entry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;

    map->count++;
  }

  FREE_ARRAY(Entry, map->entries, map->capacity);

  map->entries = entries;
  map->capacity = capacity;
}

void map_set(Map* map, const char* key, int32_t value) {
  if (map->count + 1 > map->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(map->capacity);
    adjust_capacity(map, capacity);
  }

  Entry* entry = find_entry(map->entries, map->capacity, key);

  if (entry->key == NULL && entry->value == -1) {
    map->count++;
  }

  entry->key = key;
  entry->value = value;
}

void map_add_all(Map* to, Map* from) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];

    if (entry->key != NULL) {
      map_set(to, entry->key, entry->value);
    }
  }
}

int32_t map_get(Map* map, const char* key) {
  Entry* entry = find_entry(map->entries, map->capacity, key);

  return entry->value;
}

void map_delete(Map* map, const char* key) {
  if (map->count == 0) return;

  // Find the entry.
  Entry* entry = find_entry(map->entries, map->capacity, key);

  // Place a tombstone in the entry.
  entry->key = NULL;
  entry->value = -2;  // -2 means tombstone
}
