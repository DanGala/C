#ifndef _LIST_H
#define _LIST_H

#include <stdlib.h>

typedef struct node node_t;

struct node
{
    void *data;
    node_t *next;
    node_t *previous;
};

typedef struct list
{
    node_t *head;
    node_t *tail;
    size_t item_size;
} list_t;

list_t *list_new(size_t item_size);
void list_destroy(list_t *list);
int list_empty(list_t *list);
size_t list_size(list_t *list);
int list_add(list_t *list, void *value);
int list_push_front(list_t *list, void *value);
void list_pop_front(list_t *list, void *dest);
int list_peek_front(list_t *list, void *dest);
int list_push_back(list_t *list, void *value);
void list_pop_back(list_t *list, void *dest);
int list_peek_back(list_t *list, void *dest);
void list_clear(list_t *list);

#endif
