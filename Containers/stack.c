#include "stack.h"
#ifdef DEBUG
#include <stdio.h>
#endif

/**
 * @brief Stack constructor
 * @return An owning pointer that points to the new stack structure on success, NULL on error 
 */
stack_t *stack_new()
{
    stack_t *new_stack;
    
    // Reserve memory for the new stack structure
    new_stack = malloc(sizeof *new_stack);
    if (new_stack != NULL)
    {
        // Also reserved memory for its internal representation
        new_stack->mem = list_new();
        if (new_stack->mem == NULL)
        {
            free(new_stack);
            new_stack = NULL;
        }
    }

    // Return a pointer to the new stack structure
#ifdef DEBUG
    printf("Created stack at %lx\n", (long unsigned int)new_stack);
#endif
    return new_stack;
}

/**
 * @brief Stack destructor
 * @param stack Pointer to the stack structure
 */
void stack_destroy(stack_t *stack)
{
    if (stack != NULL)
    {
        // Destroy the internal storage before freeing the memory allocated to the stack structure
#ifdef DEBUG
        printf("Destroying stack...\n");
#endif
        if (stack->mem != NULL)
            list_destroy(stack->mem);
        free(stack);
#ifdef DEBUG
        printf("Destroyed stack at %lx\n", (long unsigned int)stack);
#endif
    }
}

/**
 * @brief Check if a stack contains no items
 * @param stack Pointer to the stack structure
 * @return 1 for empty, 0 otherwise
 */
int stack_empty(stack_t *stack)
{
    return list_empty(stack->mem);
}

/**
 * @brief Check the number of items a stack contains
 * @param stack Pointer to the stack structure
 * @return Number of items contained in the stack
 */
size_t stack_size(stack_t *stack)
{
    return list_size(stack->mem);
}

/**
 * @brief Push a new data item onto a stack
 * @param stack Pointer to the stack structure
 * @param data New data item
 * @param data_size Size of data in bytes
 * @return 0 on success, -1 on error
 */
int stack_push(stack_t *stack, void *data, size_t data_size)
{
    // Default stack behavior is pushing to the back
    return list_push_back(stack->mem, data, data_size);
}

/**
 * @brief Pop a data item from a stack
 * @param stack Pointer to the stack structure
 * @param dest Destination
 */
void stack_pop(stack_t *stack, void *dest)
{
    // Default stack behavior is popping from the back
    return list_pop_back(stack->mem, dest);
}

/**
 * @brief Peek the last item pushed onto a stack
 * @param stack Pointer to the stack structure
 * @param dest Destination
 * @return 0 on success, -1 on error
 */
int stack_peek(stack_t *stack, void *dest)
{
    // Last item pushed is at the back
    return list_peek_back(stack->mem, dest);
}

/**
 * @brief Get a pointer to the top element in a stack
 * @param stack Pointer to the stack structure
 * @return A pointer to the top element in the stack structure
 */
node_t *stack_top(stack_t *stack)
{
    return stack->mem->tail;
}

/**
 * @brief Get a pointer to the bottom element in a stack
 * @param stack Pointer to the stack structure
 * @return A pointer to the bottom element in the stack structure
 */
node_t *stack_bottom(stack_t *stack)
{
    return stack->mem->head;
}

/**
 * @brief Clear a stack's contents
 * @param stack Pointer to the stack structure
 */
void stack_clear(stack_t *stack)
{
#ifdef DEBUG
    printf("Clearing stack...\n");
#endif
    list_clear(stack->mem);
}

/**
 * @brief Exchanges the contents of two stacks
 * @param stacka First stack
 * @param stackb Second stack
 * @note This function swaps the underlyinh containers of both stacks
 */
void stack_swap(stack_t *stacka, stack_t *stackb)
{
    list_t *temp;
    temp = stacka->mem;
    stacka->mem = stackb->mem;
    stackb->mem = temp;
#ifdef DEBUG
    printf("Swapped contents of stack at %lx and stack at %lx\n", (long unsigned int)stacka, (long unsigned int)stackb);
#endif
}
