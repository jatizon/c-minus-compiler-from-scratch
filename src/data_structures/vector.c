
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "vector.h"


Vector vector_new(size_t element_size) {
    return (Vector){
        .size = 0,
        .capacity = 0,
        .element_size = element_size,
        .elements = NULL,
    };
}

size_t vector_get_size(Vector* vector) {
    return vector->size;
}

size_t vector_get_capacity(Vector* vector) {
    return vector->capacity;
}

void vector_resize(Vector* vector) {
    size_t new_capacity;
    if (vector->capacity == 0)
        new_capacity = 1;
    else
        new_capacity = 2*vector->capacity;
    size_t new_capacity_bytes = new_capacity * vector->element_size;

    vector->elements = realloc(vector->elements, new_capacity_bytes);

    vector->capacity = new_capacity;
}

void* vector_get_element_ptr(Vector* vector, size_t index) {
    assert(index < vector->size && "Access out of bounds");

    void* element_ptr = (char*) vector->elements + (vector->element_size * index);
    return element_ptr;
}

void vector_set_element(Vector* vector, size_t index, void* element) {
    assert(index < vector->size && "Access out of bounds");

    void* target = vector_get_element_ptr(vector, index);
    memcpy(target, element, vector->element_size);
}

void vector_push(Vector* vector, void* element) {
    size_t new_size = vector->size + 1;
    if (new_size > vector->capacity)
        vector_resize(vector);

    vector->size = new_size;

    size_t last_position = new_size-1;
    vector_set_element(vector, last_position, element);
}

void* vector_pop(Vector* vector) {
    assert(vector->size > 0 && "Cannot pop from empty vector");

    void* last = vector_get_element_ptr(vector, vector->size-1);
    vector->size --;
    return last;
}

void vector_remove(Vector* vector, size_t index) {
    assert(index < vector->size && "Access out of bounds");

    for (size_t i = index; i < vector->size-1; ++ i) {
        void* next_element = vector_get_element_ptr(vector, i+1);
        vector_set_element(vector, i, next_element);
    }

    vector->size --;
}

int vector_find(Vector* vector, void* element, bool (*compare_func)(void*, void*)) {
    for (size_t i = 0; i < vector_get_size(vector); ++ i) {
        void* current_element_ptr = vector_get_element_ptr(vector, i);
        if (compare_func(element, current_element_ptr))
            return i;
    }

    return -1;
}
