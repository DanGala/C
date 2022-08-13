#include "forward_list.h"
#include <string.h>

/**
 * @brief Node constructor
 * @param data Data to be stored within the new node
 * @param type_size Size of the datatype stored within the new node in bytes
 * @return An owning pointer that points to the new node
 * @note Internal use only
 */
static node_t *node_new(void *data, size_t type_size)
{
    node_t *new_node;

    // Empty list items aren't supported
    if ((data == NULL) || (type_size == 0))
        return NULL;

    // Reserve memory for the new node
    new_node = malloc(sizeof *new_node);
    if (new_node != NULL)
    {
        // Also reserve memory for the encapsulated data
        new_node->data = malloc(type_size);
        if (new_node->data == NULL)
        {
            free(new_node);
            return NULL;
        }
        // Initialize the structure
        memcpy(new_node->data, data, type_size);
        new_node->next = NULL;
    }

    // Return a pointer to the new node structure
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
    }
}

/**
 * @brief Forward list constructor
 * @param item_size Size of the items contained in the list in bytes
 * @return An owning pointer that points to the new list
 */
forward_list_t *forward_list_new(size_t item_size)
{
    forward_list_t *new_list;

    // Empty list items aren't supported
    if (item_size == 0)
        return NULL;

    // Reserve memory for the new list structure
    new_list = malloc(sizeof *new_list);
    if (new_list != NULL)
    {
        // Initialize the structure
        new_list->head = NULL;
        new_list->item_size = item_size;
    }

    // Return a pointer to the new list structure
    return new_list;
}

/**
 * @brief Forward list destructor
 * @param list Pointer to the list structure to be destroyed
 */
void forward_list_destroy(forward_list_t *list)
{
    if (list != NULL)
    {
        // Destroy all nodes before freeing the memory allocated to the list structure
        forward_list_clear(list);
        free(list);
    }
}

/**
 * @brief Check if a list contains no items
 * @param list Pointer to the list structure
 * @return 1 for empty, 0 otherwise
 */
int forward_list_empty(forward_list_t *list)
{
    // A non-empty list must always have a head
    return (list->head == NULL) ? 1 : 0;
}

/**
 * @brief Check the number of items a list contains
 * @param list Pointer to the list structure
 * @return Number of items contained in the list
 */
size_t forward_list_size(forward_list_t *list)
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
 * @param value Data to be stored within the new item
 * @return 0 on success, -1 on error
 */
int forward_list_add(forward_list_t *list, void *value)
{
    // Add operation defaults to pushing back
    return forward_list_push_back(list, value);
}

/**
 * @brief Insert an item at the front of a list
 * @param list Pointer to the list structure
 * @param value Data to be stored within the new item
 * @return 0 on success, -1 on error
 */
int forward_list_push_front(forward_list_t *list, void *value)
{
    node_t *new_item;

    // Create a new node to encapsulate the data
    new_item = node_new(value, list->item_size);
    if (new_item == NULL)
        return -1;

    // The new node becomes head
    new_item->next = list->head;
    list->head = new_item;

    return 0;
}

/**
 * @brief Extract the item at the front of a list
 * @param list Pointer to the list structure
 * @param dest Destination
 */
void forward_list_pop_front(forward_list_t *list, void *dest)
{
    node_t *popped_node;

    // The list must exist and have at least one item to pop
    if ((list == NULL) || (list->head == NULL))
        return;

    // The next node on the list becomes the new head
    popped_node = list->head;
    list->head = popped_node->next;
    
    // Data from the popped node is copied into destination if provided
    if (dest != NULL)
    {
        memcpy(dest, popped_node->data, list->item_size);
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
int forward_list_peek_front(forward_list_t *list, void *dest)
{
    if ((list != NULL) && (list->head != NULL) && (dest != NULL))
    {
        // Copy peeked data into its destination
        memcpy(dest, list->head->data, list->item_size);
        return 0;
    }
    return -1;
}

/**
 * @brief Insert an item at the back of a list
 * @param list Pointer to the list structure
 * @param value Data to be stored within the new item
 * @return 0 on success, -1 on error
 */
int forward_list_push_back(forward_list_t *list, void *value)
{
    node_t *new_item;
    node_t *current_item;

    // Create a new node to encapsulate the data
    new_item = node_new(value, list->item_size);
    if (new_item == NULL)
        return -1;

    // If the list was empty, the new node becomes head
    current_item = list->head;
    if (current_item == NULL)
    {
        list->head = new_item;
    }
    else
    {
        // Find the last item on the list
        while (current_item->next != NULL)
        {
            current_item = current_item->next;
        }
        // The new node becomes the next item to that one
        current_item->next = new_item;
    }

    return 0;
}

/**
 * @brief Extract the item at the back of a list
 * @param list Pointer to the list structure
 * @param dest Destination
 */
void forward_list_pop_back(forward_list_t *list, void *dest)
{
    node_t *iterator;
    node_t *popped_node;

    // The list must exist and have at least one item to pop
    if ((list == NULL) || (list->head == NULL))
        return;

    // Find the last item on the list
    iterator = list->head;
    while (iterator->next != NULL)
    {
        if (iterator->next->next == NULL)
        {
            // The second to last node shall no longer have a node after it
            popped_node = iterator->next;
            iterator->next = NULL;
            break;
        }
        iterator = iterator->next;
    }
    
    // If the list is now empty, head must be NULL
    if (iterator == list->head)
    {
        list->head = NULL;
    }

    // Data from the popped node is copied into destination if provided
    if (dest != NULL)
    {
        memcpy(dest, popped_node->data, list->item_size);
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
int forward_list_peek_back(forward_list_t *list, void *dest)
{
    node_t *iterator;

    if ((list != NULL) && (list->head != NULL) && (dest != NULL))
    {
        // Find the last item on the list
        iterator = list->head;
        while (iterator->next != NULL)
        {
            iterator = iterator->next;
        }
        // Copy peeked data into its destination
        memcpy(dest, iterator->data, list->item_size);
        return 0;
    }
    return -1;
}

/**
 * @brief Clear a list's contents
 * @param list Pointer to the list structure
 */
void forward_list_clear(forward_list_t *list)
{
    while (forward_list_empty(list) == 0)
    {
        // Destroy nodes by popping them into oblivion
        forward_list_pop_front(list, NULL);
    }
}
