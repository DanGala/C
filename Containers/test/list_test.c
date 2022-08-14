#include "list.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Print each node in order of traversal of a list of chars
 * @param list Pointer to the list structure
 */
static void char_list_print(list_t *list)
{
    node_t * iterator;

    if (list != NULL)
    {
        printf("[ ");
        iterator = list->head;
        while (iterator != NULL)
        {
            printf("%s ", (const char *)iterator->data);
            iterator = iterator->next;
        }
        printf("]\n");
    }
}


int main(int argc, char **argv)
{
    list_t *list;
    
    // Create a list of chars
    list = list_new();
    printf("List empty? %s\n", forward_list_empty(list) ? "Yes" : "No");
    printf("List length: %zu\n", forward_list_size(list));

    // Insert some nodes
    printf("Inserting some nodes...\n");
    list_add(list, "N", 2); // include null-terminator for printing
    list_add(list, "B", 2);
    list_add(list, "X", 2);
    list_add(list, "D", 2);
    list_add(list, "H", 2);
    list_add(list, "F", 2);

    // Print the list's contents
    char_list_print(list);
    printf("List empty? %s\n", list_empty(list) ? "Yes" : "No");
    printf("List length: %zu\n", list_size(list));

    // Add and remove some more nodes
    printf("Adding 'C'\n");
    list_add(list, "C", 2);
    char_list_print(list);
    printf("Pushing 'J' to the front\n");
    list_push_front(list, "J", 2);
    char_list_print(list);
    printf("Popping twice. First from the front, then from the back\n");
    list_pop_front(list, NULL);
    char_list_print(list);
    list_pop_back(list, NULL);
    char_list_print(list);

    // Clear the list
    list_clear(list);

    // Add a node
    printf("Adding 'C'\n");
    list_add(list, "C", 2);
    char_list_print(list);

    // Destroy the list
    list_destroy(list);

    return 0;
}