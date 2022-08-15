#ifndef _STACK_H
#define _STACK_H

#include "list.h"

typedef struct stack
{
    list_t * mem;
} stack_t;

stack_t *stack_new();
void stack_destroy(stack_t *stack);
int stack_empty(stack_t *stack);
size_t stack_size(stack_t *stack);
int stack_push(stack_t *stack, void *data, size_t data_size);
void stack_pop(stack_t *stack, void *dest);
int stack_peek(stack_t *stack, void *dest);
node_t *stack_top(stack_t *stack);
node_t *stack_bottom(stack_t *stack);
void stack_clear(stack_t *stack);
void stack_swap(stack_t *stacka, stack_t *stackb);

#endif