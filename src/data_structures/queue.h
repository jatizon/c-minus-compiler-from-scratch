#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include "src/data_structures/vector.h"


typedef struct Queue {
    size_t first;
    size_t end;
    size_t size;
    Vector vector;
} Queue;

Queue queue_new(size_t element_size);

void queue_push(Queue* queue, void* element);

void* queue_pop(Queue* queue);

void* queue_front(Queue* queue);

bool queue_empty(Queue* queue);

bool queue_full(Queue* queue);

#endif
