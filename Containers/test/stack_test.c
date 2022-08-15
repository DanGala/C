#include "stack.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Print each node in order of extraction of a stack of strings into a formatted string
 * @param stack Pointer to the stack structure
 */
static void string_stack_sprint(stack_t *stack, char * dest)
{
    node_t * iterator;
    char * write_pointer = dest;

    if ((stack != NULL) && (dest != NULL))
    {
        write_pointer += sprintf(write_pointer, "[ ");
        iterator = stack_top(stack);
        while (iterator != NULL)
        {
            write_pointer += sprintf(write_pointer, "%s ", (const char *)iterator->data);
            iterator = iterator->previous;
        }
        sprintf(write_pointer, "]");
    }
}


int main(int argc, char **argv)
{
    stack_t *stack, *stack2;
    int error = 0;
    size_t pushed_nodes = 0;
    size_t pushed_nodes2 = 0;
    char popped_value[2] = { 0 };
    char peeked_value[2] = { 0 };
    char test_buffer[512] = { 0 };
    char test_buffer2[512] = { 0 };

    printf("\n--- Stack module unit test begins ---\n\n");
    
    // Create a stack of strings
    printf("Creating a stack...\n");
    stack = stack_new();
    if (stack == NULL)
    {
        fprintf(stderr, "Error in stack creation\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    if (!stack_empty(stack) || (stack_size(stack) != 0))
    {
        fprintf(stderr, "Error: stack wasn't empty upon creation\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Push some nodes
    printf("Pushing some nodes...\n");
    error |= stack_push(stack, "N", 2); // include null-terminator for printing
    pushed_nodes++;
    error |= stack_push(stack, "B", 2);
    pushed_nodes++;
    error |= stack_push(stack, "X", 2);
    pushed_nodes++;
    error |= stack_push(stack, "D", 2);
    pushed_nodes++;
    error |= stack_push(stack, "H", 2);
    pushed_nodes++;
    error |= stack_push(stack, "F", 2);
    pushed_nodes++;

    if (error)
    {
        fprintf(stderr, "Error pushing nodes to the stack\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Print the stack's contents
    string_stack_sprint(stack, test_buffer);
    if (strncmp("[ F H D X B N ]", test_buffer, sizeof("[ F H D X B N ]")) != 0)
    {
        fprintf(stderr, "Error: stack contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t[ F H D X B N ]\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    if (stack_empty(stack) || (stack_size(stack) != pushed_nodes))
    {
        fprintf(stderr, "Error: stack empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", stack_empty(stack) ? "Empty" : "Not empty", stack_size(stack));
        fprintf(stderr, "Expected:\n\tNot empty (%zu items)\n", pushed_nodes);
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Remove some more nodes
    printf("Popping twice...\n");
    // Make sure both peek() and pop() store the output if a destination buffer is provided
    stack_peek(stack, peeked_value); // doesn't modify the stack
    stack_pop(stack, popped_value);
    if (strncmp(peeked_value, popped_value, sizeof(peeked_value)) != 0)
    {
        fprintf(stderr, "Error: stack popped data doesn't match previously peeked data\n");
        fprintf(stderr, "Peeked: %s\t", peeked_value);
        fprintf(stderr, "Popped: %s\n", popped_value);
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    pushed_nodes--;
    // Print the stack's contents
    string_stack_sprint(stack, test_buffer);
    if (strncmp("[ H D X B N ]", test_buffer, sizeof("[ H D X B N ]")) != 0)
    {
        fprintf(stderr, "Error: stack contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t[ H D X B N ]\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (stack_empty(stack) || (stack_size(stack) != pushed_nodes))
    {
        fprintf(stderr, "Error: stack empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", stack_empty(stack) ? "Empty" : "Not empty", stack_size(stack));
        fprintf(stderr, "Expected:\n\tNot empty (%zu items)\n", pushed_nodes);
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    // Let's also test pop() without a destination buffer
    stack_pop(stack, NULL);
    pushed_nodes--;
    // Print the stack's contents
    string_stack_sprint(stack, test_buffer);
    if (strncmp("[ D X B N ]", test_buffer, sizeof("[ D X B N ]")) != 0)
    {
        fprintf(stderr, "Error: stack contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t[ D X B N ]\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (stack_empty(stack) || (stack_size(stack) != pushed_nodes))
    {
        fprintf(stderr, "Error: stack empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", stack_empty(stack) ? "Empty" : "Not empty", stack_size(stack));
        fprintf(stderr, "Expected:\n\tNot empty (%zu items)\n", pushed_nodes);
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    // And make sure peek() fails if a destination buffer is not provided
    if (stack_peek(stack, NULL) == 0)
    {
        fprintf(stderr, "Error: stack_peek() should return an error if invoked without a destination buffer\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }

    // Check top and bottom
    if (stack_top(stack) != stack->mem->tail)
    {
        fprintf(stderr, "Error: 'stack_top()' function not working correctly\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (stack_bottom(stack) != stack->mem->head)
    {
        fprintf(stderr, "Error: 'stack_bottom()' function not working correctly\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
#ifdef DEBUG
    printf("Top is: %s (%lx)\n", (const char *)stack_top(stack)->data, (long unsigned int)stack_top(stack));
    printf("Bottom is: %s (%lx)\n", (const char *)stack_bottom(stack)->data, (long unsigned int)stack_bottom(stack));
#endif

    // Clear the stack
    printf("Clearing stack...\n");
    stack_clear(stack);
    pushed_nodes = 0;
    // Print the stack's contents
    string_stack_sprint(stack, test_buffer);
    if (strncmp("[ ]", test_buffer, sizeof("[ ]")) != 0)
    {
        fprintf(stderr, "Error: stack contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s\n", test_buffer);
        fprintf(stderr, "Expected:\n\t[ ]\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    if (!stack_empty(stack) || (stack_size(stack) != 0))
    {
        fprintf(stderr, "Error: stack empty/size do not match expectations\n");
        fprintf(stderr, "Actual:\n\t%s (%zu items)\n", stack_empty(stack) ? "Empty" : "Not empty", stack_size(stack));
        fprintf(stderr, "Expected:\n\tEmpty (%u items)\n", 0);
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));

    // Push an item
    printf("Pushing an item...\n");
    if (stack_push(stack, "C", 2) != 0)
    {
        fprintf(stderr, "Error: push to stack failed after clearing the stack\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    pushed_nodes++;

    // Create a new stack
    printf("Creating a second stack...\n");
    stack2 = stack_new();
    printf("Pushing some nodes...\n");
    stack_push(stack2, "D", 2);
    pushed_nodes2++;
    stack_push(stack2, "H", 2);
    pushed_nodes2++;

    // Swap contents
    printf("Swapping contents of two stacks...\n");
    stack_swap(stack, stack2);
    // Print the stack's contents
    string_stack_sprint(stack, test_buffer);
    string_stack_sprint(stack2, test_buffer2);
    if ((strncmp("[ H D ]", test_buffer, sizeof("[ H D ]")) != 0) || (strncmp("[ C ]", test_buffer2, sizeof("[ C ]")) != 0))
    {
        fprintf(stderr, "Error: stack contents do not match expectations\n");
        fprintf(stderr, "Actual:\n\tStack 1: %s\n\tStack 2: %s\n", test_buffer, test_buffer2);
        fprintf(stderr, "Expected:\n\tStack 1: [ H D ]\n\tStack 2: [ C ]\n");
        printf("\n--- Stack module unit test ends. Test result: FAILURE! ---\n");
        exit(1);
    }
    // Clear buffer for next test
    memset(test_buffer, 0, sizeof(test_buffer));
    memset(test_buffer2, 0, sizeof(test_buffer2));

    // Destroy the stacks
    printf("Cleaning up...\n");
    stack_destroy(stack);
    stack_destroy(stack2);

    printf("\n--- Stack module unit test ends. Test result: SUCCESS! ---\n");
    return 0;
}