#include "list.h"
#include <string.h>
#ifdef DEBUG
#include <stdio.h>
#endif

/**
 * @brief Node constructor
 * @param data Data to be stored within the new node
 * @param data_size Size of the datatype stored within the new node in bytes
 * @return An owning pointer that points to the new node
 * @note Internal use only
 */
static node_t *node_new(void *data, size_t data_size)
{
    node_t *new_node;

    // Empty list items aren't supported
    if ((data == NULL) || (data_size == 0))
        return NULL;

    // Reserve memory for the new node
    new_node = malloc(sizeof *new_node);
    if (new_node != NULL)
    {
        // Also reserve memory for the encapsulated data
        new_node->data = malloc(data_size);
        if (new_node->data == NULL)
        {
            free(new_node);
            return NULL;
        }
        // Initialize the structure
        memcpy(new_node->data, data, data_size);
        new_node->data_size = data_size;
        new_node->previous = NULL;
        new_node->next = NULL;
    }

    // Return a pointer to the new node structure
#ifdef DEBUG
    printf("Created node at %lx\n", (unsigned long int)new_node);
#endif
    return new_node;
}

/**
 * @brief Node destructor
 * @param node Pointer to the node structure to be destroyed
 * @note Internal use only
 */
static void node_destroy(node_t *node)
{
    if (node != NULL)
    {
        // Free memory allocated to the node structure and its internal data
        free(node->data);
        free(node);
#ifdef DEBUG
        printf("Destroyed node at %lx\n", (unsigned long int)node);
#endif
    }
}

/**
 * @brief List constructor
 * @return An owning pointer that points to the new list
 */
list_t *list_new()
{
    list_t *new_list;

    // Reserve memory for the new list structure
    new_list = malloc(sizeof *new_list);
    if (new_list != NULL)
    {
        // Initialize the structure
        new_list->head = NULL;
        new_list->tail = NULL;
    }

    // Return a pointer to the new list structure
#ifdef DEBUG
    printf("Created new list at %lx\n", (unsigned long int)new_list);
#endif
    return new_list;
}

/**
 * @brief List destructor
 * @param list Pointer to the list structure to be destroyed
 */
void list_destroy(list_t *list)
{
    if (list != NULL)
    {
        // Destroy all nodes before freeing the memory allocated to the list structure
#ifdef DEBUG
    printf("Destroying list...\n");
#endif
        list_clear(list);
        free(list);
#ifdef DEBUG
    printf("Destroyed list at %lx\n", (unsigned long int)list);
#endif
    }
}

/**
 * @brief Check if a list contains no items
 * @param list Pointer to the list structure
 * @return 1 for empty, 0 otherwise
 */
int list_empty(list_t *list)
{
    // A non-empty list must always have a head
    return (list->head == NULL) ? 1 : 0;
}

/**
 * @brief Check the number of items a list contains
 * @param list Pointer to the list structure
 * @return Number of items contained in the list
 */
size_t list_size(list_t *list)
{
    size_t ret = 0;
    node_t *iterator;
    
    // Traverse the list counting the nodes
    iterator = list->head;
    while (iterator != NULL)
    {
        ret++;
        iterator = iterator->next;
    }

    return ret;
}

/**
 * @brief Add an item to a list
 * @param list Pointer to the list structure
 * @param data Data to be stored within the new item
 * @param data_size Size of data in bytes
 * @return 0 on success, -1 on error
 */
int list_add(list_t *list, void *data, size_t data_size)
{
    // Add operation defaults to pushing back
    return list_push_back(list, data, data_size);
}

/**
 * @brief Insert an item at the front of a list
 * @param list Pointer to the list structure
 * @param data Data to be stored within the new item
 * @param data_size Size of data in bytes
 * @return 0 on success, -1 on error
 */
int list_push_front(list_t *list, void *data, size_t data_size)
{
    node_t *new_item;
    
    // Create a new node to encapsulate the data
    new_item = node_new(data, data_size);
    if (new_item == NULL)
        return -1;
    
    // If the list was empty, the new node becomes head and tail
    if (list->head == NULL)
    {
        list->head = new_item;
        list->tail = new_item;
    }
    else
    {
        // Otherwise, the new node becomes head
        new_item->next = list->head;
        list->head->previous = new_item;
        list->head = new_item;
    }
    
    return 0;
}

/**
 * @brief Extract the item at the front of a list
 * @param list Pointer to the list structure
 * @param dest Destination
 */
void list_pop_front(list_t *list, void *dest)
{
    node_t *popped_node;

    // The list must exist and have at least one item to pop
    if ((list == NULL) || (list->tail == NULL))
        return;

    // The next node on the list becomes the new head
    popped_node = list->head;
    list->head = popped_node->next;
    if (list->head)
    {
        list->head->previous = NULL;
    }
    else
    {
        // If the list is now empty, tail must also be NULL
        list->tail = NULL;
    }

    // Data from the popped node is copied into destination if provided
    if (dest != NULL)
    {
        memcpy(dest, popped_node->data, popped_node->data_size);
    }
    // Finally, the popped node is destroyed
    node_destroy(popped_node);
}

/**
 * @brief Peek the item at the front of a list
 * @param list Pointer to the list structure
 * @param dest Destination
 * @return 0 on success, -1 on error
 */
int list_peek_front(list_t *list, void *dest)
{
    if ((list != NULL) && (list->head != NULL) && (dest != NULL))
    {
        // Copy peeked data into its destination
        memcpy(dest, list->head->data, list->head->data_size);
        return 0;
    }
    return -1;
}

/**
 * @brief Insert an item at the back of a list
 * @param list Pointer to the list structure
 * @param data Data to be stored within the new item
 * @param data_size Size of data in bytes
 * @return 0 on success, -1 on error
 */
int list_push_back(list_t *list, void *data, size_t data_size)
{
    node_t *new_item;
    
    // Create a new node to encapsulate the data
    new_item = node_new(data, data_size);
    if (new_item == NULL)
        return -1;

    // If the list was empty, the new node becomes head and tail
    new_item->previous = list->tail;
    if (new_item->previous == NULL)
    {
        list->head = new_item;
        list->tail = new_item;
    }
    else
    {
        // Otherwise, the new node becomes tail
        list->tail->next = new_item;
        list->tail = new_item;
    }

    return 0;
}

/**
 * @brief Extract the item at the back of a list
 * @param list Pointer to the list structure
 * @param dest Destination
 */
void list_pop_back(list_t *list, void *dest)
{
    node_t *popped_node;

    // The list must exist and have at least one item to pop
    if ((list == NULL) || (list->tail == NULL))
        return;

    // The second to last node on the list becomes the new tail
    popped_node = list->tail;
    list->tail = popped_node->previous;
    if (list->tail != NULL)
    {
        list->tail->next = NULL;
    }
    else
    {
        // If the list is now empty, head must also be NULL
        list->head = NULL;
    }

    // Data from the popped node is copied into destination if provided
    if (dest != NULL)
    {
        memcpy(dest, popped_node->data, popped_node->data_size);
    }
    // Finally, the popped node is destroyed
    node_destroy(popped_node);
}

/**
 * @brief Peek the item at the back of a list
 * @param list Pointer to the list structure
 * @param dest Destination
 * @return 0 on success, -1 on error
 */
int list_peek_back(list_t *list, void *dest)
{
    if ((list != NULL) && (list->tail != NULL) && (dest != NULL))
    {
        // Copy peeked data into its destination
        memcpy(dest, list->tail->data, list->tail->data_size);
        return 0;
    }
    return -1;
}

/**
 * @brief Clear a list's contents
 * @param list Pointer to the list structure
 */
void list_clear(list_t *list)
{
#ifdef DEBUG
    printf("Clearing list...\n");
#endif
    while (list_empty(list) == 0)
    {
        // Destroy nodes by popping them into oblivion
        list_pop_front(list, NULL);
    }
    list->head = NULL;
    list->tail = NULL;
}
