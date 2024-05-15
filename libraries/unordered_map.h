#ifndef UNORDERED_MAP_H_
#define UNORDERED_MAP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct _hash_table unordered_map;
typedef uint64_t(hashfunction)(unordered_map*, const uint8_t*, size_t);

unordered_map* unordered_map_new_store_objects(size_t keylen, void (*deleter)(void* obj));
unordered_map* unordered_map_new_store_values(size_t keylen);
void unordered_map_delete(unordered_map* self);
void unordered_map_clear(unordered_map* self);
void unordered_map_print(unordered_map* self);
bool unordered_map_insert(unordered_map* self, const void* key, void* obj);
void* unordered_map_lookup(unordered_map* self, const void* key);
void* unordered_map_erase(unordered_map* self, const void* key);
size_t unordered_map_size(unordered_map* self);

void unordered_map_test();

#endif