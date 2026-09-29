#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stddef.h>
#include "src/data_structures/vector.h"


typedef struct Hashmap {
    size_t size;
    size_t capacity;
    size_t element_size;
    float load_factor_threshold;
    size_t num_buckets;
    Vector* buckets;
    size_t (*hash_function)(void*, size_t);
    bool (*compare_func)(void*, void*);
} Hashmap;

size_t hashmap_get_num_buckets_for_capacity(Hashmap* hashmap, size_t capacity);

Hashmap hashmap_new(
    size_t (*hash_function)(void*, size_t),
    float load_factor_threshold,
    size_t element_size
);

void hashmap_initialize_bucket(Vector* buckets, size_t index);

void hashmap_rehash(Hashmap* hashmap, size_t new_num_buckets);

void hashmap_resize(Hashmap* hashmap);

void hashmap_set(Hashmap* hashmap, char* key, void* element);

bool hashmap_contains(Hashmap* hashmap, char* key);

void* hashmap_get(Hashmap* hashmap, char* key);

void hashmap_remove(Hashmap* hashmap, char* key);

#endif
