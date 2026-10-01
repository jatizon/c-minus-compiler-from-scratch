#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>


typedef struct Vector {
    size_t size;
    size_t capacity;
    size_t element_size;
    void* elements;
} Vector;

Vector vector_new(size_t element_size);

Vector vector_new_with_capacity(size_t element_size, size_t capacity);

void vector_free(Vector* vector);

size_t vector_next_capacity(Vector* vector);

void vector_resize(Vector* vector);

void* vector_get_element_ptr(Vector* vector, size_t index);

void* vector_get_element_ptr_unsafe(Vector* vector, size_t index);

void vector_set(Vector* vector, size_t index, void* element);

void vector_set_unsafe(Vector* vector, size_t index, void* element);

void vector_push(Vector* vector, void* element);

void* vector_pop(Vector* vector);

void vector_remove(Vector* vector, size_t index);

int vector_find(Vector* vector, void* element, bool (*compare_func)(void*, void*));

bool vector_empty(Vector* vector);

size_t vector_get_size(Vector* vector);

void* vector_get_data(Vector* vector);

#endif
