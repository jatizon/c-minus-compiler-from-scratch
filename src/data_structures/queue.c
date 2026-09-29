#include "src/data_structures/queue.h"


Queue queue_new(size_t element_size) {
    return (Queue) {
        .first = 0,
        .end = 0,
        .size = 0,
        .vector = vector_new(element_size),
    };
}

void queue_push(Queue* queue, void* element) {
    if (!queue_full(queue)) {
        size_t insert_index = queue->end;
        vector_set_unsafe(&queue->vector, insert_index, element);

        queue->end = (insert_index + 1) % queue->vector.capacity;
        queue->size ++;
        return;
    }

    size_t new_capacity = vector_next_capacity(&queue->vector);
    Vector new_vector = vector_new_with_capacity(queue->vector.element_size, new_capacity);

    size_t offset;
    for (offset = 0; offset < queue->size; ++ offset) {
        size_t old_index = (queue->first + offset) % queue->vector.capacity;

        void* old_element = vector_get_element_ptr_unsafe(&queue->vector, old_index);
        vector_set_unsafe(&new_vector, offset, old_element);
    }

    vector_set_unsafe(&new_vector, offset, element);

    queue->first = 0;
    queue->end = (offset + 1) % new_capacity;
    queue->size ++;

    vector_free(&queue->vector);
    queue->vector = new_vector;
}

void* queue_pop(Queue* queue) {
    void* front = queue_front(queue);
    queue->first = (queue->first + 1) % queue->vector.capacity;
    queue->size --;
    return front;
}

void* queue_front(Queue* queue) {
    void* front = vector_get_element_ptr_unsafe(&queue->vector, queue->first);
    return front;
}

bool queue_empty(Queue* queue) {
    return queue->size == 0;
}

bool queue_full(Queue* queue) {
    return queue->size == queue->vector.capacity;
}