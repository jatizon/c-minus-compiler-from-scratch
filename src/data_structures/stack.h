#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stddef.h>
#include "src/data_structures/vector.h"


typedef struct Stack {
    Vector vector;
} Stack;

Stack stack_new(size_t element_size);

void stack_push(Stack* stack, void* element);

void* stack_pop(Stack* stack);

void* stack_top(Stack* stack);

bool stack_empty(Stack* stack);

size_t stack_get_size(Stack* stack);

#endif
