#ifndef _QUEUE_H
#define _QUEUE_H

#include "list.h"

typedef struct queue
{
    list_t * mem;
} queue_t;

queue_t *queue_new();
void queue_destroy(queue_t *queue);
int queue_empty(queue_t *queue);
size_t queue_size(queue_t *queue);
int queue_push(queue_t *queue, void *data, size_t data_size);
void queue_pop(queue_t *queue, void *dest);
int queue_peek(queue_t *queue, void *dest);
node_t *queue_front(queue_t *queue);
node_t *queue_back(queue_t *queue);
void queue_clear(queue_t *queue);
void queue_swap(queue_t *queuea, queue_t *queueb);

#endif