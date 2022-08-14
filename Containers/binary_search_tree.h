#ifndef _BINARY_SEARCH_TREE_H
#define _BINARY_SEARCH_TREE_H

typedef struct bst_node bst_node_t;

struct bst_node
{
    void *key;
    bst_node_t *parent;
    bst_node_t *left;
    bst_node_t *right;
    int key_size;
};

typedef int (*compare_func_t) (void *k1, int ks1, void *k2, int ks2);

typedef struct bst_tree
{
    bst_node_t *root;
    compare_func_t compare;
} bst_tree_t;

bst_node_t *bst_node_new(bst_node_t *parent, void *key, int key_size);
void bst_node_destroy(bst_node_t * node);
bst_tree_t *bst_tree_new(compare_func_t compare);
void bst_tree_destroy(bst_tree_t * tree);
bst_node_t *bst_tree_insert(bst_tree_t *tree, bst_node_t *current, bst_node_t *parent, void *key, int key_size);
bst_node_t *bst_tree_search(bst_tree_t *tree, bst_node_t *root, void *key, int key_size);

#endif