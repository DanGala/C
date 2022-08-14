#ifndef _SORTED_LIST_H
#define _SORTED_LIST_H

#include <stdlib.h>

typedef struct node node_t;

struct node
{
    void *data;
    size_t data_size;
    node_t *next;
};

typedef int (*cmp_func_t) (void *data1, int data1_size, void *data2, int data2_size);
typedef void (*sort_func_t) (node_t **head_ref, cmp_func_t compare);

typedef struct sorted_list
{
    node_t *head;
    cmp_func_t compare;
    sort_func_t sort;
} sorted_list_t;

sorted_list_t *sorted_list_new(cmp_func_t compare, sort_func_t sort);
void sorted_list_destroy(sorted_list_t *list);
int sorted_list_empty(sorted_list_t *list);
size_t sorted_list_size(sorted_list_t *list);
int sorted_list_insert(sorted_list_t *list, void *data, size_t data_size);
void sorted_list_pop_front(sorted_list_t *list, void *dest);
int sorted_list_peek_front(sorted_list_t *list, void *dest);
void sorted_list_pop_back(sorted_list_t *list, void *dest);
int sorted_list_peek_back(sorted_list_t *list, void *dest);
void sorted_list_clear(sorted_list_t *list);

#endif