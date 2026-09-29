#include "src/data_structures/vector.h"


typedef struct Stack {
    Vector vector;
} Stack;

Stack stack_new(size_t element_size) {
    return (Stack) {
        .vector = vector_new(element_size),
    };
}

void stack_push(Stack* stack, void* element) {
    vector_push(&stack->vector, element);
}

void* stack_pop(Stack* stack) {
    return vector_pop(&stack->vector);
}

void* stack_top(Stack* stack) {
    return vector_get_element_ptr(&stack->vector, stack->vector.size-1);
}

bool stack_empty(Stack* stack) {
    return vector_empty(&stack->vector);
}