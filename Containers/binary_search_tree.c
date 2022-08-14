#include "binary_search_tree.h"
#include <string.h>
#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif

/**
 * @brief BST node constructor
 * @param parent Pointer to an optional parent node
 * @param key New node's key
 * @param key_size Size of the key in bytes
 * @return An owning pointer that points to the new node
 */
bst_node_t *bst_node_new(bst_node_t *parent, void *key, int key_size)
{
    bst_node_t *new_node;

    // Empty keys aren't supported
    if ((key == NULL) || (key_size == 0))
    {
        return NULL;
    }

    // Reserve memory for the new node
    new_node = malloc(sizeof *new_node);
    if (new_node != NULL)
    {
        // Also reserve memory for the key
        new_node->key = malloc(key_size);
        if (new_node->key == NULL)
        {
            free(new_node);
            return NULL;
        }

        // Initialize the structure
        memcpy(new_node->key, key, key_size);
        new_node->parent = parent;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->key_size = key_size;
    }

    // Return a pointer to the new structure
    return new_node;
}

/**
 * @brief BST node destructor
 * @param node Pointer to node structure
 */
void bst_node_destroy(bst_node_t * node)
{
    if (node != NULL)
    {
        if (node->key != NULL)
            free(node->key);
        free(node);
#ifdef DEBUG
        printf("Destroyed node at %lx\n", (long unsigned int)node);
#endif
    }
}

/**
 * @brief BST tree constructor
 * @param compare Key comparison function
 * @return An owning pointer that points to the new tree
 */
bst_tree_t *bst_tree_new(compare_func_t compare)
{
    bst_tree_t *new_tree;

    // Reserve memory for the new tree
    new_tree = malloc(sizeof *new_tree);
    if (new_tree != NULL)
    {
        // Initialize the tree structure
        new_tree->root = NULL;
        new_tree->compare = compare;
    }
    
    // Return a pointer to the new structure
#ifdef DEBUG
    printf("Created new tree at %lx\n", (long unsigned int)new_tree);
#endif
    return new_tree;
}

/**
 * @brief Destroy all nodes in a tree
 * @param node Pointer to the current tree's root
 */
static void bst_tree_destroy_all(bst_node_t *node)
{
    bst_node_t *parent;

    if (node == NULL)
        return;

    // A node can be destroyed when it has no remaining links to any children
    parent = node->parent;
    if ((node->left == NULL) && (node->right == NULL))
    {
        if (parent != NULL)
        {
            // Remove links from parent node
            if (parent->left == node)
            {
                parent->left = NULL;
            }
            else
            {
                parent->right = NULL;
            }
            
            // Destroy this node's parent's subtree
            bst_tree_destroy_all(parent);
        }
        // Destroy this node
        bst_node_destroy(node);
    }
    else if (node->left)
    {
        bst_tree_destroy_all(node->left);
    }
    else
    {
        bst_tree_destroy_all(node->right);
    }
}

void bst_tree_destroy(bst_tree_t * tree)
{
#ifdef DEBUG
    printf("Destroying tree at %lx\n", (long unsigned int)tree);
#endif
    bst_tree_destroy_all(tree->root);
    free(tree);
#ifdef DEBUG
    printf("Tree at %lx has been destroyed\n", (long unsigned int)tree);
#endif
}

/**
 * @brief Insert a new node with a given key into a tree
 * @param tree Pointer to tree structure
 * @param current Pointer to current node
 * @param parent Pointer to current node's parent node
 * @param key New node's key
 * @param key_size Size of the key in bytes
 * @return A pointer to the current node after the insertion
 */
bst_node_t *bst_tree_insert(bst_tree_t *tree, bst_node_t * current, bst_node_t *parent, void *key, int key_size)
{
    // If the tree is empty, return a new node
    if (current == NULL)
    {
        return bst_node_new(parent, key, key_size);
    }
    else
    {
        if (tree->compare(current->key, current->key_size, key, key_size) > 0)
        {
            // If the key is smaller than the current key, descend into the left subtree
            current->left = bst_tree_insert(tree, current->left, current, key, key_size);
        }
        else // Assumes no duplicates
        {
            // If the key is greater than the current key, descend into the right subtree
            current->right = bst_tree_insert(tree, current->right, current, key, key_size);
        }
    }

    // Return the unchanged current node pointer
    return current;
}

/**
 * @brief Top-down search for a given key inside a tree
 * @param tree Pointer to the tree structure
 * @param root Pointer to the current subtree's root node
 * @param key Key to search for
 * @param key_size Size of the key in bytes
 * @return A pointer to the tree node that matches the given key, or NULL if the key wasn't found
 */
bst_node_t *bst_tree_search(bst_tree_t *tree, bst_node_t *root, void *key, int key_size)
{
    if ((key == NULL) || (key_size == 0))
    {
        return NULL;
    }

    // If root is NULL or key is present at root node
    if ((root == NULL) || (tree->compare(root->key, root->key_size, key, key_size) == 0))
    {
        return root;
    }

    // If the key is smaller than the root key, continue searching the left subtree
    if (tree->compare(root->key, root->key_size, key, key_size) > 0)
    {
        return bst_tree_search(tree, root->left, key, key_size);
    }

    // The key is greater than the root key, continue searching the right subtree
    return bst_tree_search(tree, root->right, key, key_size);
}
