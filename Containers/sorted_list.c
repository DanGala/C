#include "sorted_list.h"
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
 * @brief Sorted list constructor
 * @param item_size Size of the items contained in the list in bytes
 * @param compare Comparison function used to decide if two items are sorted
 * @param sort Sorting algorithm to be used on the list
 * @return An owning pointer that points to the new list
 */
sorted_list_t* sorted_list_new(size_t item_size, cmp_func_t compare, sort_func_t sort)
{
    sorted_list_t *new_list;

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
        new_list->compare = compare;
        new_list->sort = sort;
    }

    // Return a pointer to the new list structure
    return new_list;
}

/**
 * @brief Sorted list destructor
 * @param list Pointer to the list structure to be destroyed
 */
void sorted_list_destroy(sorted_list_t *list)
{
    if (list != NULL)
    {
        // Destroy all nodes before freeing the memory allocated to the list structure
        sorted_list_clear(list);
        free(list);
    }
}

/**
 * @brief Check if a list contains no items
 * @param list Pointer to the list structure
 * @return 1 for empty, 0 otherwise
 */
int sorted_list_empty(sorted_list_t *list)
{
    // A non-empty list must always have a head
    return (list->head == NULL) ? 1 : 0;
}

/**
 * @brief Check the number of items a list contains
 * @param list Pointer to the list structure
 * @return Number of items contained in the list
 */
size_t sorted_list_size(sorted_list_t *list)
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
 * @brief Insert an item into a list
 * @param list Pointer to the list structure
 * @param value Data to be stored within the new item
 * @return 0 on success, -1 on error
 */
int sorted_list_insert(sorted_list_t *list, void *value)
{
    node_t *new_item;

    // Create a new node to encapsulate the data
    new_item = node_new(value, list->item_size);
    if (new_item == NULL)
        return -1;

    // Insert the new node at the front of the list
    new_item->next = list->head;
    list->head = new_item;

    // Sort the list using its comparison function and sorting algorithm
    list->sort(list->head, list->compare);

    return 0;
}

/**
 * @brief Extract the item at the front of a list
 * @param list Pointer to the list structure
 * @param dest Destination
 */
void sorted_list_pop_front(sorted_list_t *list, void *dest)
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
int sorted_list_peek_front(sorted_list_t *list, void *dest)
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
 * @brief Extract the item at the back of a list
 * @param list Pointer to the list structure
 * @param dest Destination
 */
void sorted_list_pop_back(sorted_list_t *list, void *dest)
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
int sorted_list_peek_back(sorted_list_t *list, void *dest)
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
void sorted_list_clear(sorted_list_t *list)
{
    while (sorted_list_empty(list) == 0)
    {
        // Destroy nodes by popping them into oblivion
        sorted_list_pop_front(list, NULL);
    }
}
