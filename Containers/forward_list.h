#ifndef _FORWARD_forward_list_H
#define _FORWARD_forward_list_H

#include <stdlib.h>

typedef struct node node_t;

struct node
{
    void *data;
    size_t data_size;
    node_t *next;
};

typedef struct forward_list
{
    node_t *head;
} forward_list_t;

forward_list_t *forward_list_new();
void forward_list_destroy(forward_list_t *list);
int forward_list_empty(forward_list_t *list);
size_t forward_list_size(forward_list_t *list);
int forward_list_add(forward_list_t *list, void *data, size_t data_size);
int forward_list_push_front(forward_list_t *list, void *data, size_t data_size);
void forward_list_pop_front(forward_list_t *list, void *dest);
int forward_list_peek_front(forward_list_t *list, void *dest);
int forward_list_push_back(forward_list_t *list, void *data, size_t data_size);
void forward_list_pop_back(forward_list_t *list, void *dest);
int forward_list_peek_back(forward_list_t *list, void *dest);
void forward_list_clear(forward_list_t *list);

#endif