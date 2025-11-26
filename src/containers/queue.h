// queue.h
#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct queue queue;

queue* queue_new(void);
void queue_free(queue* q);

void queue_push(queue* q, void* item);
void* queue_pop(queue* q);

bool queue_empty(queue* q);
size_t queue_length(queue* q);

#endif
