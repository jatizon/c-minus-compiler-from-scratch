#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "src/data_structures/hashmap.h"
#include "src/data_structures/vector.h"
#include "assert.h"
#include "src/helpers/hash.h"
#include "src/helpers/math.h"


typedef struct Entry {
    char* key;
    void* value;
} Entry;

Entry entry_borrow(char* key, void* value) {
    return (Entry) {
        .key = key,
        .value = value,
    };
}

Entry entry_clone(char* key, void* value, size_t value_size) {
    void* value_copy;
    if (value_size != 0) {
        value_copy = malloc(value_size);
        memcpy(value_copy, value, value_size);
    }
    else value_copy = NULL;

    return (Entry) {
        .key = strdup(key),
        .value = value_copy,
    };
}

void entry_free_fields(Entry* entry) {
    free(entry->key);
    free(entry->value);
}

size_t hashmap_get_num_buckets_for_capacity(Hashmap* hashmap, size_t capacity) {
    int num_buckets = ceil(capacity / hashmap->load_factor_threshold);
    int num_buckets_prime = next_prime(num_buckets);
    return num_buckets_prime;
}

Hashmap hashmap_new(
    size_t (*hash_function)(void*, size_t),
    float load_factor_threshold,
    size_t element_size
) {
    return (Hashmap) {
        .size = 0,
        .capacity = 0,
        .element_size = element_size,
        .load_factor_threshold = load_factor_threshold,
        .num_buckets = 0,
        .buckets = NULL,
        .hash_function = hash_function,
    };
}

void hashmap_initialize_bucket(Vector* buckets, size_t index) {
    buckets[index] = vector_new(sizeof(Entry));
}

void hashmap_rehash(Hashmap* hashmap, size_t new_num_buckets) {
    Vector* new_buckets = (Vector*) malloc(new_num_buckets * sizeof(Vector));
    for (size_t bucket_index = 0; bucket_index < new_num_buckets; ++ bucket_index)
        hashmap_initialize_bucket(new_buckets, bucket_index);

    for (size_t bucket_index = 0; bucket_index < hashmap->num_buckets; ++ bucket_index)
        while (hashmap->buckets[bucket_index].size > 0 ) {
            Entry* entry = vector_pop(&hashmap->buckets[bucket_index]);

            size_t new_bucket_index = hashmap->hash_function(entry->key, new_num_buckets);
            Vector* new_bucket = &new_buckets[new_bucket_index];

            vector_push(new_bucket, entry);
        }
    
    free(hashmap->buckets);
    hashmap->buckets = new_buckets;
}

void hashmap_resize(Hashmap* hashmap) {
    size_t new_capacity;
    if (hashmap->capacity == 0)
        new_capacity = 1;
    else
        new_capacity = 2*hashmap->capacity;

    size_t new_num_buckets = hashmap_get_num_buckets_for_capacity(hashmap, new_capacity);

    hashmap->capacity = new_capacity;

    hashmap_rehash(hashmap, new_num_buckets);
    hashmap->num_buckets = new_num_buckets;
}

bool entry_compare_keys(void* entry1, void* entry2) {
    Entry* typed_entry1 = entry1;
    Entry* typed_entry2 = entry2;

    return strcmp(typed_entry1->key, typed_entry2->key) == 0; 
}

void hashmap_set(Hashmap* hashmap, char* key, void* element) {
    size_t new_size = hashmap->size + 1;
    if (new_size > hashmap->capacity)
        hashmap_resize(hashmap);

    size_t bucket_index = hashmap->hash_function(key, hashmap->num_buckets);
    Vector* bucket = &hashmap->buckets[bucket_index];

    Entry lookup_entry = entry_borrow(key, NULL);

    int found_index = vector_find(bucket, &lookup_entry, entry_compare_keys);
    if (found_index != -1) {
        Entry* found = vector_get_element_ptr(bucket, found_index);
        Entry cloned = entry_clone(key, element, hashmap->element_size);
        found->value = cloned.value;
        return;
    }

    Entry entry = entry_clone(key, element, hashmap->element_size);
    vector_push(bucket, &entry);

    hashmap->size = new_size;
}

bool hashmap_contains(Hashmap* hashmap, char* key) {
    if (hashmap->num_buckets == 0)
        return false;

    size_t bucket_index = hashmap->hash_function(key, hashmap->num_buckets);
    Vector* bucket = &hashmap->buckets[bucket_index];

    Entry entry = entry_borrow(key, NULL);

    int found_index = vector_find(bucket, &entry, entry_compare_keys);
    return (found_index != -1);
}

void* hashmap_get(Hashmap* hashmap, char* key) {
    if (hashmap->num_buckets == 0)
        return NULL;

    size_t bucket_index = hashmap->hash_function(key, hashmap->num_buckets);
    Vector* bucket = &hashmap->buckets[bucket_index];

    Entry entry = entry_borrow(key, NULL);

    int found_index = vector_find(bucket, &entry, entry_compare_keys);
    if (found_index == -1)
        return NULL;

    Entry* found = vector_get_element_ptr(bucket, found_index);
    return found->value;
}

void hashmap_remove(Hashmap* hashmap, char* key) {
    if (hashmap->num_buckets == 0)
        return;

    size_t bucket_index = hashmap->hash_function(key, hashmap->num_buckets);
    Vector* bucket = &hashmap->buckets[bucket_index];

    Entry entry = entry_borrow(key, NULL);

    int found_index = vector_find(bucket, &entry, entry_compare_keys);
    if (found_index == -1)
        return;

    vector_remove(bucket, found_index);
    hashmap->size --;
}