#include "priority_queue.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Print each node in order of dequeueing in a queue of strings into a formatted string
 * @param queue Pointer to the queue structure
 */
static void string_priority_queue_sprint(priority_queue_t *queue, char * dest)
{
    node_t * iterator;
    char * write_pointer = dest;

    if ((queue != NULL) && (dest != NULL))
    {
        write_pointer += sprintf(write_pointer, "<- [ ");
        iterator = priority_queue_front(queue);
        while (iterator != NULL)
        {
            write_pointer += sprintf(write_pointer, "%s ", (const char *)iterator->data);
            iterator = iterator->next;
        }
        sprintf(write_pointer, "] <-");
    }
}

/**
 * @brief Perform string comparison on data
 * @param data1 First data
 * @param data1_size First data's length
 * @param data2 Second data
 * @param data2_size Second data's length
 * @return 0 if data are equal, >0 if data1 > data2, <0 if data2 > data1
 */
static int string_compare (void *data1, int data1_size, void *data2, int data2_size)
{
    return strcmp((const char *)data1, (const char *)data2);
}

/**
 * @brief Perform string comparison on data
 * @param data1 First data
 * @param data1_size First data's length
 * @param data2 Second data
 * @param data2_size Second data's length
 * @return 0 if data are equal, <0 if data1 > data2, >0 if data2 > data1
 */
static int inverse_string_compare (void *data1, int data1_size, void *data2, int data2_size)
{
    return -strcmp((const char *)data1, (const char *)data2);
}

/**
 * @brief Split the nodes of the given list into front and back halves
 * @param source Head of the sublist
 * @param front_ref Reference to the front half start
 * @param back_ref Reference to the back half start
 * @note If the length is odd, the extra node should go in the front list
 * @note Uses fast/slow pointer strategy
 */
static void front_back_split(node_t *source, node_t **front_ref, node_t **back_ref)
{
    node_t *fast;
    node_t *slow;

    if ((source == NULL) || (front_ref == NULL) || (back_ref == NULL))
        return;

    slow = source;
    fast = source->next;

    // Advance 'fast' two nodes and 'slow' one node
    while (fast != NULL)
    {
        fast = fast->next;
        if (fast != NULL)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split in two at that point
    *front_ref = source;
    *back_ref = slow->next;
    slow->next = NULL;
}

/**
 * @brief Perform a sorted merge on two lists with a given comparison function for their nodes
 * @param front First of the merging lists
 * @param back Second of the merging lists
 * @param compare Comparison function
 * @return A pointer to the result of the merge
 */
static node_t *sorted_merge(node_t *front, node_t *back, cmp_func_t compare)
{
    node_t *ret_node;

    // Base cases
    if (front == NULL)
        return back;
    if (back == NULL)
        return front;

    // Pick either front or back and recur
    if (compare(front->data, front->data_size, back->data, back->data_size) <= 0)
    {
        ret_node = front;
        ret_node->next = sorted_merge(front->next, back, compare);
    }
    else
    {
        ret_node = back;
        ret_node->next = sorted_merge(front, back->next, compare);
    }

    return ret_node;
}

/**
 * @brief Apply the mergesort sorting algorithm to a singly-linked list
 * @param head_ref Reference to the head of the list
 * @param compare Comparison function used for merging lists
 */
void merge_sort(node_t **head_ref, cmp_func_t compare)
{
    node_t *front;
    node_t *back;

    // If head is NULL or there is only one element in the linked list then then list is sorted
    if ((head_ref == NULL) || (*head_ref == NULL) || ((*head_ref)->next == NULL) || (compare == NULL))
        return;

    // Divide the linked list into two halves
    front_back_split(*head_ref, &front, &back);

    // Sort the two halves
    merge_sort(&front, compare);
    merge_sort(&back, compare);

    // Merge the sorted halves and update head
    *head_ref = sorted_merge(front, back, compare);
}

int main(int argc, char **argv)
{
    priority_queue_t *queue, *queue2;
    int error = 0;
    size_t pushed_nodes = 0;
    size_t pushed_nodes2 = 0;
    char popped_value[2] = { 0 };
    char peeked_value[2] = { 0 };
    char test_buffer[512] = { 0 };
    char test_buffer2[512] = { 0 };

    printf("\n--- Priority queue module unit test begins ---\n\n");
    printf("Priority queue implemented as a FIFO: <- [ front | X | X | X | X | back ] <-\n\n");
    
    // Create a queue of strings
    printf("Creating a queue...\n");
    queue = priority_queue_new(string_compare, merge_sort);
    if (queue == NULL)
    {
        fprintf(stderr, "Error in queue creation\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    if (!priority_queue_empty(queue) || (priority_queue_size(queue) != 0))
    {
        fprintf(stderr, "Error: queue wasn't empty upon creation\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Push some items
    printf("Pushing some items...\n");
    error |= priority_queue_push(queue, "N", 2); // include null-terminator for printing
    pushed_nodes++;
    error |= priority_queue_push(queue, "B", 2);
    pushed_nodes++;
    error |= priority_queue_push(queue, "X", 2);
    pushed_nodes++;
    error |= priority_queue_push(queue, "D", 2);
    pushed_nodes++;
    error |= priority_queue_push(queue, "H", 2);
    pushed_nodes++;
    error |= priority_queue_push(queue, "F", 2);
    pushed_nodes++;

    if (error)
    {
        fprintf(stderr, "Error pushing items to the queue\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Print the queue's contents
    string_priority_queue_sprint(queue, test_buffer);
    if (strncmp("<- [ B D F H N X ] <-", test_buffer, sizeof("<- [ B D F H N X ] <-")) != 0)
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t<- [ B D F H N X ] <-\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    if (priority_queue_empty(queue) || (priority_queue_size(queue) != pushed_nodes))
    {
        fprintf(stderr, "Error: queue empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", priority_queue_empty(queue) ? "Empty" : "Not empty", priority_queue_size(queue));
        fprintf(stderr, "Expected:\n\tNot empty (%zu items)\n", pushed_nodes);
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Remove some more items
    printf("Popping twice...\n");
    // Make sure both peek() and pop() store the output if a destination buffer is provided
    priority_queue_peek(queue, peeked_value); // doesn't modify the queue
    priority_queue_pop(queue, popped_value);
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
    string_priority_queue_sprint(queue, test_buffer);
    if (strncmp("<- [ D F H N X ] <-", test_buffer, sizeof("<- [ D F H N X ] <-")) != 0)
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t<- [ D F H N X ] <-\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (priority_queue_empty(queue) || (priority_queue_size(queue) != pushed_nodes))
    {
        fprintf(stderr, "Error: queue empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", priority_queue_empty(queue) ? "Empty" : "Not empty", priority_queue_size(queue));
        fprintf(stderr, "Expected:\n\tNot empty (%zu items)\n", pushed_nodes);
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    // Let's also test pop() without a destination buffer
    priority_queue_pop(queue, NULL);
    pushed_nodes--;
    // Print the queue's contents
    string_priority_queue_sprint(queue, test_buffer);
    if (strncmp("<- [ F H N X ] <-", test_buffer, sizeof("<- [ F H N X ] <-")) != 0)
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t<- [ F H N X ] <-\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (priority_queue_empty(queue) || (priority_queue_size(queue) != pushed_nodes))
    {
        fprintf(stderr, "Error: queue empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", priority_queue_empty(queue) ? "Empty" : "Not empty", priority_queue_size(queue));
        fprintf(stderr, "Expected:\n\tNot empty (%zu items)\n", pushed_nodes);
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    // And make sure peek() fails if a destination buffer is not provided
    if (priority_queue_peek(queue, NULL) == 0)
    {
        fprintf(stderr, "Error: priority_queue_peek() should return an error if invoked without a destination buffer\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Check front and back
#ifdef DEBUG
    printf("Front is: %s (%lx)\n", (const char *)priority_queue_front(queue)->data, (long unsigned int)priority_queue_front(queue));
    printf("Back is: %s (%lx)\n", (const char *)priority_queue_back(queue)->data, (long unsigned int)priority_queue_back(queue));
#endif
    if (priority_queue_front(queue) != queue->mem->head)
    {
        fprintf(stderr, "Error: 'priority_queue_front()' function not working correctly\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Clear the queue
    printf("Clearing queue...\n");
    priority_queue_clear(queue);
    pushed_nodes = 0;
    // Print the queue's contents
    string_priority_queue_sprint(queue, test_buffer);
    if (strncmp("<- [ ] <-", test_buffer, sizeof("<- [ ] <-")) != 0)
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t<- [ ] <-\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (!priority_queue_empty(queue) || (priority_queue_size(queue) != 0))
    {
        fprintf(stderr, "Error: queue empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", priority_queue_empty(queue) ? "Empty" : "Not empty", priority_queue_size(queue));
        fprintf(stderr, "Expected:\n\tEmpty (%u items)\n", 0);
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    // Push an item
    printf("Pushing some items...\n");
    if (priority_queue_push(queue, "C", 2) != 0)
    {
        fprintf(stderr, "Error: push to queue failed after clearing the queue\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    pushed_nodes++;
    priority_queue_push(queue, "A", 2);
    pushed_nodes++;

    // Create a new queue with inverted priorities
    printf("Creating a second queue...\n");
    queue2 = priority_queue_new(inverse_string_compare, merge_sort);
    printf("Pushing some items...\n");
    priority_queue_push(queue2, "D", 2);
    pushed_nodes2++;
    priority_queue_push(queue2, "H", 2);
    pushed_nodes2++;
    // Ensure priority is reversed on this queue
    string_priority_queue_sprint(queue2, test_buffer2);
    if (strncmp("<- [ H D ] <-", test_buffer2, sizeof("<- [ H D ] <-")) != 0)
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer2);
        fprintf(stderr, "Expected:\n\t<- [ H D ] <-\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer2, 0, sizeof(test_buffer2));

    // Swap contents
    printf("Swapping contents of two queues...\n");
    priority_queue_swap(queue, queue2);
    // Print the queue's contents
    string_priority_queue_sprint(queue, test_buffer);
    string_priority_queue_sprint(queue2, test_buffer2);
    // Ensure not only queues have been swapped, but sorting meets the new priority scheme
    if ((strncmp("<- [ D H ] <-", test_buffer, sizeof("<- [ D H ] <-")) != 0) || (strncmp("<- [ C A ] <-", test_buffer2, sizeof("<- [ C A ] <-")) != 0))
    {
        fprintf(stderr, "Error: queue contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\tqueue 1: %s\n\tqueue 2: %s\n", test_buffer, test_buffer2);
        fprintf(stderr, "Expected:\n\tqueue 1: <- [ D H ] <-\n\tqueue 2: <- [ C A ] <-\n");
        printf("\n--- Queue module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));
    memset(test_buffer2, 0, sizeof(test_buffer2));

    // Destroy the queues
    printf("Cleaning up...\n");
    priority_queue_destroy(queue);
    priority_queue_destroy(queue2);

    printf("\n--- Queue module unit test ends. Test result: SUCCESS! ---\n");
    return 0;
}