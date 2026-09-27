#ifndef HASH_HELPERS_H
#define HASH_HELPERS_H

#include <stddef.h>

size_t djb2_hash(void* key, size_t key_size);

#endif
