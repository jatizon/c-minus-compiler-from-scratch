
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "src/data_structures/vector.h"


Vector vector_new(size_t element_size) {
    return (Vector){
        .size = 0,
        .capacity = 0,
        .element_size = element_size,
        .elements = NULL,
    };
}

Vector vector_new_with_capacity(size_t element_size, size_t capacity) {
    return (Vector){
        .size = 0,
        .capacity = capacity,
        .element_size = element_size,
        .elements = malloc(capacity * element_size),
    };
}

bool vector_empty(Vector* vector) {
    return vector->size == 0;
}

void vector_free(Vector* vector) {
    free(vector->elements);
}

size_t vector_next_capacity(Vector* vector) {
    size_t new_capacity;
    if (vector->capacity == 0)
        new_capacity = 1;
    else
        new_capacity = 2*vector->capacity;
    return new_capacity;
}

void vector_resize(Vector* vector) {
    size_t new_capacity = vector_next_capacity(vector);
    size_t new_capacity_bytes = new_capacity * vector->element_size;

    vector->elements = realloc(vector->elements, new_capacity_bytes);

    vector->capacity = new_capacity;
}

void* vector_get_element_ptr(Vector* vector, size_t index) {
    assert(index < vector->size && "Access out of bounds");

    void* element_ptr = (char*) vector->elements + (vector->element_size * index);
    return element_ptr;
}

void* vector_get_element_ptr_unsafe(Vector* vector, size_t index) {
    assert(index < vector->capacity && "Access out of bounds");

    return (char*) vector->elements + (vector->element_size * index);
}

void vector_set(Vector* vector, size_t index, void* element) {
    assert(index < vector->size && "Access out of bounds");

    void* target = vector_get_element_ptr(vector, index);
    memcpy(target, element, vector->element_size);
}

void vector_set_unsafe(Vector* vector, size_t index, void* element) {
    assert(index < vector->capacity && "Access out of bounds");

    void* target = (char*) vector->elements + (vector->element_size * index);
    memcpy(target, element, vector->element_size);
}

void vector_push(Vector* vector, void* element) {
    size_t new_size = vector->size + 1;
    if (new_size > vector->capacity)
        vector_resize(vector);

    vector->size = new_size;

    size_t last_position = new_size-1;
    vector_set(vector, last_position, element);
}

void* vector_pop(Vector* vector) {
    if (vector->size == 0)
        return NULL;

    void* last = vector_get_element_ptr(vector, vector->size-1);
    vector->size --;
    return last;
}

void vector_remove(Vector* vector, size_t index) {
    assert(index < vector->size && "Access out of bounds");

    for (size_t i = index; i < vector->size-1; ++ i) {
        void* next_element = vector_get_element_ptr(vector, i+1);
        vector_set(vector, i, next_element);
    }

    vector->size --;
}

int vector_find(Vector* vector, void* element, bool (*compare_func)(void*, void*)) {
    for (size_t i = 0; i < vector->size; ++ i) {
        void* current_element_ptr = vector_get_element_ptr(vector, i);
        if (compare_func(element, current_element_ptr))
            return i;
    }

    return -1;
}
