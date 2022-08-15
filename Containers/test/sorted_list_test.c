#include "sorted_list.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Print each node in order of traversal of a list of chars
 * @param list Pointer to the list structure
 */
static void char_list_print(sorted_list_t *list)
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
    sorted_list_t *list;
    
    // Create a sorted list of strings, using merge sort as the sorting algorithm
    list = sorted_list_new(string_compare, merge_sort);
    printf("List empty? %s\n", sorted_list_empty(list) ? "Yes" : "No");
    printf("List length: %zu\n", sorted_list_size(list));

    // Insert some nodes
    printf("Inserting some nodes...\n");
    printf("Insert 'N'\n");
    sorted_list_insert(list, "N", 2); // include null-terminator for printing
    printf("Insert 'B'\n");
    sorted_list_insert(list, "B", 2);
    printf("Insert 'X'\n");
    sorted_list_insert(list, "X", 2);
    printf("Insert 'D'\n");
    sorted_list_insert(list, "D", 2);
    printf("Insert 'H'\n");
    sorted_list_insert(list, "H", 2);
    printf("Insert 'F'\n");
    sorted_list_insert(list, "F", 2);

    // Print the list's contents
    printf("\nListing nodes:\n");
    char_list_print(list);
    printf("List empty? %s\n", sorted_list_empty(list) ? "Yes" : "No");
    printf("List length: %zu\n", sorted_list_size(list));

    // Add and remove some more nodes
    printf("Inserting 'C'\n");
    sorted_list_insert(list, "C", 2);
    char_list_print(list);
    printf("Inserting 'J'\n");
    sorted_list_insert(list, "J", 2);
    char_list_print(list);
    printf("Popping twice. First from the front, then from the back\n");
    sorted_list_pop_front(list, NULL);
    char_list_print(list);
    sorted_list_pop_back(list, NULL);
    char_list_print(list);

    // Clear list
    sorted_list_clear(list);

    // Add a node
    printf("Adding 'C'\n");
    sorted_list_insert(list, "C", 2);
    char_list_print(list);

    // Destroy the list
    sorted_list_destroy(list);

    return 0;
}