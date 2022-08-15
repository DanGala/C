#include "priority_queue.h"

#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

/**
 * @brief Piority queue constructor
 * @param compare Comparison function used to decide which of two items has priority
 * @param sort Sorting algorithm to be used on the underlying container
 * @return An owning pointer that points to the new queue structure on success, NULL on error 
 */
priority_queue_t *priority_queue_new(cmp_func_t compare, sort_func_t sort)
{
    priority_queue_t *new_queue;
    
    // Reserve memory for the new queue structure
    new_queue = malloc(sizeof *new_queue);
    if (new_queue != NULL)
    {
        // Also reserved memory for its internal representation
        new_queue->mem = sorted_list_new(compare, sort);
        if (new_queue->mem == NULL)
        {
            free(new_queue);
            new_queue = NULL;
        }
        new_queue->compare = compare;
        new_queue->sort = sort;
    }

    // Return a pointer to the new queue structure
#ifdef DEBUG
    printf("Created queue at %lx\n", (long unsigned int)new_queue);
#endif
    return new_queue;
}

/**
 * @brief Queue destructor
 * @param queue Pointer to the queue structure
 */
void priority_queue_destroy(priority_queue_t *queue)
{
    if (queue != NULL)
    {
        // Destroy the internal storage before freeing the memory allocated to the queue structure
#ifdef DEBUG
        printf("Destroying queue...\n");
#endif
        if (queue->mem != NULL)
            sorted_list_destroy(queue->mem);
        free(queue);
#ifdef DEBUG
        printf("Destroyed queue at %lx\n", (long unsigned int)queue);
#endif
    }
}

/**
 * @brief Check if a queue contains no items
 * @param queue Pointer to the queue structure
 * @return 1 for empty, 0 otherwise
 */
int priority_queue_empty(priority_queue_t *queue)
{
    return sorted_list_empty(queue->mem);
}

/**
 * @brief Check the number of items a queue contains
 * @param queue Pointer to the queue structure
 * @return Number of items contained in the queue
 */
size_t priority_queue_size(priority_queue_t *queue)
{
    return sorted_list_size(queue->mem);
}

/**
 * @brief Push a new data item into a queue
 * @param queue Pointer to the queue structure
 * @param data New data item
 * @param data_size Size of data in bytes
 * @return 0 on success, -1 on error
 */
int priority_queue_push(priority_queue_t *queue, void *data, size_t data_size)
{
    return sorted_list_insert(queue->mem, data, data_size);
}

/**
 * @brief Pop a data item from a queue
 * @param queue Pointer to the queue structure
 * @param dest Destination
 */
void priority_queue_pop(priority_queue_t *queue, void *dest)
{
    // Default queue behavior is popping from the front
    return sorted_list_pop_front(queue->mem, dest);
}

/**
 * @brief Peek the next item to be popped from a queue
 * @param queue Pointer to the queue structure
 * @param dest Destination
 * @return 0 on success, -1 on error
 */
int priority_queue_peek(priority_queue_t *queue, void *dest)
{
    // Next item to be popped is at the front
    return sorted_list_peek_front(queue->mem, dest);
}

/**
 * @brief Get a pointer to the next element in a queue
 * @param queue Pointer to the queue structure
 * @return A pointer to the top element in the queue structure
 */
node_t *priority_queue_front(priority_queue_t *queue)
{
    return queue->mem->head;
}

/**
 * @brief Get a pointer to the last element in a queue
 * @param queue Pointer to the queue structure
 * @return A pointer to the bottom element in the queue structure
 */
node_t *priority_queue_back(priority_queue_t *queue)
{
    node_t *iterator = NULL;

    if (queue != NULL)
    {
        // Find the last item on the list
        iterator = queue->mem->head;
        while (iterator->next != NULL)
        {
            iterator = iterator->next;
        }
    }

    return iterator;
}

/**
 * @brief Clear a queue's contents
 * @param queue Pointer to the queue structure
 */
void priority_queue_clear(priority_queue_t *queue)
{
#ifdef DEBUG
    printf("Clearing queue...\n");
#endif
    sorted_list_clear(queue->mem);
}

/**
 * @brief Exchanges the contents of two queues
 * @param queuea First queue
 * @param queueb Second queue
 * @note This function swaps the underlyinh containers of both queues
 */
void priority_queue_swap(priority_queue_t *queuea, priority_queue_t *queueb)
{
    sorted_list_t *temp;

    // Swap the underlying containers
    temp = queuea->mem;
    queuea->mem = queueb->mem;
    queueb->mem = temp;

    // Update the container's comparison and sorting functions
    queuea->mem->compare = queuea->compare;
    queueb->mem->compare = queueb->compare;
    queuea->mem->sort = queuea->sort;
    queueb->mem->sort = queueb->sort;

    // Sort the queues using their comparison functions and sorting algorithms
    queuea->mem->sort(&queuea->mem->head, queuea->compare);
    queueb->mem->sort(&queueb->mem->head, queueb->compare);
#ifdef DEBUG
    printf("Swapped contents of queue at %lx and queue at %lx\n", (long unsigned int)queuea, (long unsigned int)queueb);
#endif
}
