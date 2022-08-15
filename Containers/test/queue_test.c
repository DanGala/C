#include "queue.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Print each node in order of dequeueing in a queue of strings into a formatted string
 * @param queue Pointer to the queue structure
 */
static void string_queue_sprint(queue_t *queue, char * dest)
{
    node_t * iterator;
    char * write_pointer = dest;

    if ((queue != NULL) && (dest != NULL))
    {
        write_pointer += sprintf(write_pointer, "-> [ ");
        iterator = queue_back(queue);
        while (iterator != NULL)
        {
            write_pointer += sprintf(write_pointer, "%s ", (const char *)iterator->data);
            iterator = iterator->previous;
        }
        sprintf(write_pointer, "] ->");
    }
}


int main(int argc, char **argv)
{
    queue_t *queue, *queue2;
    int error = 0;
    size_t pushed_nodes = 0;
    size_t pushed_nodes2 = 0;
    char popped_value[2] = { 0 };
    char peeked_value[2] = { 0 };
    char test_buffer[512] = { 0 };
    char test_buffer2[512] = { 0 };

    printf("\n--- Queue module unit test begins ---\n\n");
    printf("Queue implemented as a FIFO: -> [ back | X | X | X | X | front ] ->\n\n");
    
    // Create a queue of strings
    printf("Creating a queue...\n");
    queue = queue_new();
    if (queue == NULL)
    {
        fprintf(stderr, "Error in queue creation\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    if (!queue_empty(queue) || (queue_size(queue) != 0))
    {
        fprintf(stderr, "Error: queue wasn't empty upon creation\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Push some nodes
    printf("Pushing some nodes...\n");
    error |= queue_push(queue, "N", 2); // include null-terminator for printing
    pushed_nodes++;
    error |= queue_push(queue, "B", 2);
    pushed_nodes++;
    error |= queue_push(queue, "X", 2);
    pushed_nodes++;
    error |= queue_push(queue, "D", 2);
    pushed_nodes++;
    error |= queue_push(queue, "H", 2);
    pushed_nodes++;
    error |= queue_push(queue, "F", 2);
    pushed_nodes++;

    if (error)
    {
        fprintf(stderr, "Error pushing nodes to the queue\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Print the queue's contents
    string_queue_sprint(queue, test_buffer);
    if (strncmp("-> [ F H D X B N ] ->", test_buffer, sizeof("-> [ F H D X B N ] ->")) != 0)
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t-> [ F H D X B N ] ->\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    if (queue_empty(queue) || (queue_size(queue) != pushed_nodes))
    {
        fprintf(stderr, "Error: queue empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", queue_empty(queue) ? "Empty" : "Not empty", queue_size(queue));
        fprintf(stderr, "Expected:\n\tNot empty (%zu items)\n", pushed_nodes);
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Remove some more nodes
    printf("Popping twice...\n");
    // Make sure both peek() and pop() store the output if a destination buffer is provided
    queue_peek(queue, peeked_value); // doesn't modify the queue
    queue_pop(queue, popped_value);
    if (strncmp(peeked_value, popped_value, sizeof(peeked_value)) != 0)
    {
        fprintf(stderr, "Error: queue popped data doesn't match previously peeked data\n");
        fprintf(stderr, "Peeked: %s\t", peeked_value);
        fprintf(stderr, "Popped: %s\n", popped_value);
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    pushed_nodes--;
    // Print the queue's contents
    string_queue_sprint(queue, test_buffer);
    if (strncmp("-> [ F H D X B ] ->", test_buffer, sizeof("-> [ F H D X B ] ->")) != 0)
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t-> [ F H D X B ] ->\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (queue_empty(queue) || (queue_size(queue) != pushed_nodes))
    {
        fprintf(stderr, "Error: queue empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", queue_empty(queue) ? "Empty" : "Not empty", queue_size(queue));
        fprintf(stderr, "Expected:\n\tNot empty (%zu items)\n", pushed_nodes);
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    // Let's also test pop() without a destination buffer
    queue_pop(queue, NULL);
    pushed_nodes--;
    // Print the queue's contents
    string_queue_sprint(queue, test_buffer);
    if (strncmp("-> [ F H D X ] ->", test_buffer, sizeof("-> [ F H D X ] ->")) != 0)
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t-> [ F H D X ] ->\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (queue_empty(queue) || (queue_size(queue) != pushed_nodes))
    {
        fprintf(stderr, "Error: queue empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", queue_empty(queue) ? "Empty" : "Not empty", queue_size(queue));
        fprintf(stderr, "Expected:\n\tNot empty (%zu items)\n", pushed_nodes);
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    // And make sure peek() fails if a destination buffer is not provided
    if (queue_peek(queue, NULL) == 0)
    {
        fprintf(stderr, "Error: queue_peek() should return an error if invoked without a destination buffer\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Check front and back
#ifdef DEBUG
    printf("Front is: %s (%lx)\n", (const char *)queue_front(queue)->data, (long unsigned int)queue_front(queue));
    printf("Back is: %s (%lx)\n", (const char *)queue_back(queue)->data, (long unsigned int)queue_back(queue));
#endif
    if (queue_front(queue) != queue->mem->head)
    {
        fprintf(stderr, "Error: 'queue_front()' function not working correctly\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (queue_back(queue) != queue->mem->tail)
    {
        fprintf(stderr, "Error: 'queue_back()' function not working correctly\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Clear the queue
    printf("Clearing queue...\n");
    queue_clear(queue);
    pushed_nodes = 0;
    // Print the queue's contents
    string_queue_sprint(queue, test_buffer);
    if (strncmp("-> [ ] ->", test_buffer, sizeof("-> [ ] ->")) != 0)
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t-> [ ] ->\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (!queue_empty(queue) || (queue_size(queue) != 0))
    {
        fprintf(stderr, "Error: queue empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", queue_empty(queue) ? "Empty" : "Not empty", queue_size(queue));
        fprintf(stderr, "Expected:\n\tEmpty (%u items)\n", 0);
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    // Push an item
    printf("Pushing an item...\n");
    if (queue_push(queue, "C", 2) != 0)
    {
        fprintf(stderr, "Error: push to queue failed after clearing the queue\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    pushed_nodes++;

    // Create a new queue
    printf("Creating a second queue...\n");
    queue2 = queue_new();
    printf("Pushing some nodes...\n");
    queue_push(queue2, "D", 2);
    pushed_nodes2++;
    queue_push(queue2, "H", 2);
    pushed_nodes2++;

    // Swap contents
    printf("Swapping contents of two queues...\n");
    queue_swap(queue, queue2);
    // Print the queue's contents
    string_queue_sprint(queue, test_buffer);
    string_queue_sprint(queue2, test_buffer2);
    if ((strncmp("-> [ H D ] ->", test_buffer, sizeof("-> [ H D ] ->")) != 0) || (strncmp("-> [ C ] ->", test_buffer2, sizeof("-> [ C ] ->")) != 0))
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\tqueue 1: %s\n\tqueue 2: %s\n", test_buffer, test_buffer2);
        fprintf(stderr, "Expected:\n\tqueue 1: -> [ H D ] ->\n\tqueue 2: -> [ C ] ->\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));
    memset(test_buffer2, 0, sizeof(test_buffer2));

    // Destroy the queues
    printf("Cleaning up...\n");
    queue_destroy(queue);
    queue_destroy(queue2);

    printf("\n--- Queue module unit test ends. Test result: SUCCESS! ---\n");
    return 0;
}