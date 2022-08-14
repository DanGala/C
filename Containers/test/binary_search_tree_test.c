#include "binary_search_tree.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Print each node in order of traversal of a BST tree/subtree containing string keys
 * @param root Root node of the subtree
 */
static void string_bst_tree_print(bst_node_t *root)
{
    if (root != NULL)
    {
        string_bst_tree_print(root->left);
        printf("\tAddress: %lx - Key: %s\n", (long unsigned int)root, (char *)root->key);
        string_bst_tree_print(root->right);
    }
}

/**
 * @brief Perform string comparison on keys
 * @param k1 First key
 * @param ks1 First key's length
 * @param k2 Second key
 * @param ks2 Second key's length
 * @return 0 if keys are equal, >0 if k1 > k2, <0 if k2 > k1
 */
static int string_compare (void *k1, int ks1, void *k2, int ks2)
{
    return strcmp((const char *)k1, (const char *)k2);
}


int main(int argc, char **argv)
{
    bst_tree_t *tree;
    
    // Create a BST using strings as keys, and string_compare as the comparison function
    tree = bst_tree_new(string_compare);

    // Insert some nodes
    printf("Inserting some nodes...\n");
    // Remember to set the tree root to the first node inserted
    tree->root = bst_tree_insert(tree, tree->root, NULL, "N", 1);
    bst_tree_insert(tree, tree->root, NULL, "B", 1);
    bst_tree_insert(tree, tree->root, NULL, "X", 1);
    bst_tree_insert(tree, tree->root, NULL, "D", 1);
    bst_tree_insert(tree, tree->root, NULL, "H", 1);
    bst_tree_insert(tree, tree->root, NULL, "F", 1);

    // Print the tree to verify the structure matches our expectations
    printf("Printing tree in traversal order:\n");
    string_bst_tree_print(tree->root);

    // See if the search function finds an existing key
    printf("\nAddress of node that contains key 'H': %lx\n", (long unsigned int)bst_tree_search(tree, tree->root, "H", 1));

    // What if the key doesn't exist?
    printf("An inexistent key returns NULL? %s\n\n", (bst_tree_search(tree, tree->root, "Z", 1) == NULL) ? "Yes" : "No");
    bst_tree_destroy(tree);

    return 0;
}