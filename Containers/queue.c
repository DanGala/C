#include "queue.h"
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

/**
 * @brief Queue constructor
 * @return An owning pointer that points to the new queue structure on success, NULL on error 
 */
queue_t *queue_new()
{
    queue_t *new_queue;
    
    // Reserve memory for the new queue structure
    new_queue = malloc(sizeof *new_queue);
    if (new_queue != NULL)
    {
        // Also reserved memory for its internal representation
        new_queue->mem = list_new();
        if (new_queue->mem == NULL)
        {
            free(new_queue);
            new_queue = NULL;
        }
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
void queue_destroy(queue_t *queue)
{
    if (queue != NULL)
    {
        // Destroy the internal storage before freeing the memory allocated to the queue structure
#ifdef DEBUG
        printf("Destroying queue...\n");
#endif
        if (queue->mem != NULL)
            list_destroy(queue->mem);
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
int queue_empty(queue_t *queue)
{
    return list_empty(queue->mem);
}

/**
 * @brief Check the number of items a queue contains
 * @param queue Pointer to the queue structure
 * @return Number of items contained in the queue
 */
size_t queue_size(queue_t *queue)
{
    return list_size(queue->mem);
}

/**
 * @brief Push a new data item onto a queue
 * @param queue Pointer to the queue structure
 * @param data New data item
 * @param data_size Size of data in bytes
 * @return 0 on success, -1 on error
 */
int queue_push(queue_t *queue, void *data, size_t data_size)
{
    // Default queue behavior is pushing to the back
    return list_push_back(queue->mem, data, data_size);
}

/**
 * @brief Pop a data item from a queue
 * @param queue Pointer to the queue structure
 * @param dest Destination
 */
void queue_pop(queue_t *queue, void *dest)
{
    // Default queue behavior is popping from the front
    return list_pop_front(queue->mem, dest);
}

/**
 * @brief Peek the last item pushed onto a queue
 * @param queue Pointer to the queue structure
 * @param dest Destination
 * @return 0 on success, -1 on error
 */
int queue_peek(queue_t *queue, void *dest)
{
    // Last item pushed is at the front
    return list_peek_front(queue->mem, dest);
}

/**
 * @brief Get a pointer to the next element in a queue
 * @param queue Pointer to the queue structure
 * @return A pointer to the top element in the queue structure
 */
node_t *queue_front(queue_t *queue)
{
    return queue->mem->head;
}

/**
 * @brief Get a pointer to the last element in a queue
 * @param queue Pointer to the queue structure
 * @return A pointer to the bottom element in the queue structure
 */
node_t *queue_back(queue_t *queue)
{
    return queue->mem->tail;
}

/**
 * @brief Clear a queue's contents
 * @param queue Pointer to the queue structure
 */
void queue_clear(queue_t *queue)
{
#ifdef DEBUG
    printf("Clearing queue...\n");
#endif
    list_clear(queue->mem);
}

/**
 * @brief Exchanges the contents of two queues
 * @param queuea First queue
 * @param queueb Second queue
 * @note This function swaps the underlyinh containers of both queues
 */
void queue_swap(queue_t *queuea, queue_t *queueb)
{
    list_t *temp;
    temp = queuea->mem;
    queuea->mem = queueb->mem;
    queueb->mem = temp;
#ifdef DEBUG
    printf("Swapped contents of queue at %lx and queue at %lx\n", (long unsigned int)queuea, (long unsigned int)queueb);
#endif
}
