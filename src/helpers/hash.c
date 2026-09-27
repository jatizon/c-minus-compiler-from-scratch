#include <stddef.h>
#include "hash.h"


size_t djb2_hash(void* key, size_t num_buckets) {
    size_t seed = 5381;

    size_t hash = seed;
    char* begin = key;
    for (char* it = begin; *it != '\0'; ++ it)
        hash = (hash * 33 + *it) % num_buckets;

    return hash;
}