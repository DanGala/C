#ifndef _PRIORITY_QUEUE_H
#define _PRIORITY_QUEUE_H

#include "sorted_list.h"

typedef struct priority_queue
{
    sorted_list_t * mem;
    cmp_func_t compare;
    sort_func_t sort;
} priority_queue_t;

priority_queue_t *priority_queue_new(cmp_func_t compare, sort_func_t sort);
void priority_queue_destroy(priority_queue_t* queue);
int priority_queue_empty(priority_queue_t* queue);
size_t priority_queue_size(priority_queue_t* queue);
int priority_queue_push(priority_queue_t* queue, void *data, size_t data_size);
void priority_queue_pop(priority_queue_t* queue, void *dest);
int priority_queue_peek(priority_queue_t* queue, void *dest);
node_t *priority_queue_front(priority_queue_t* queue);
node_t *priority_queue_back(priority_queue_t* queue);
void priority_queue_clear(priority_queue_t* queue);
void priority_queue_swap(priority_queue_t* queuea, priority_queue_t* queueb);

#endif