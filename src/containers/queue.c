#include "containers/queue.h"

#include <stdlib.h>
#include <stdio.h>

struct queue {
    void** data;
    size_t cap;
    size_t len;
    size_t head;
    size_t tail;
};

#define QUEUE_INITIAL_CAP 16

static void queue_ensure_capacity(queue* q, size_t min_cap) {
    if (!q) return;
    if (q->cap >= min_cap) return;

    size_t new_cap = (q->cap > 0) ? q->cap : QUEUE_INITIAL_CAP;
    while (new_cap < min_cap) {
        new_cap *= 2;
    }

    void** new_data = malloc(new_cap * sizeof(void*));
    if (!new_data) {
        fprintf(stderr, "queue_ensure_capacity: out of memory\n");
        abort();
    }

    for (size_t i = 0; i < q->len; i++) {
        const size_t idx = (q->head + i) % q->cap;
        new_data[i] = q->data[idx];
    }

    free(q->data);
    q->data = new_data;
    q->cap = new_cap;
    q->head = 0;
    q->tail = q->len;
}

queue* queue_new(void) {
    queue* q = malloc(sizeof(queue));
    if (!q) {
        fprintf(stderr, "queue_new: out of memory\n");
        abort();
    }

    q->cap = 0;
    q->len = 0;
    q->head = 0;
    q->tail = 0;
    q->data = NULL;

    queue_ensure_capacity(q, QUEUE_INITIAL_CAP);
    return q;
}

void queue_free(queue* q) {
    if (!q) return;
    free(q->data);
    free(q);
}

void queue_push(queue* q, void* item) {
    if (!q) return;

    if (q->len >= q->cap) {
        queue_ensure_capacity(q, q->len + 1);
    }

    q->data[q->tail] = item;
    q->tail = (q->tail + 1) % q->cap;
    q->len++;
}

void* queue_pop(queue* q) {
    if (!q) return NULL;
    if (q->len == 0) return NULL;

    void* item = q->data[q->head];
    q->head = (q->head + 1) % q->cap;
    q->len--;
    return item;
}

bool queue_empty(queue* q) {
    if (!q) return true;
    return q->len == 0;
}

size_t queue_length(queue* q) {
    if (!q) return 0;
    return q->len;
}
