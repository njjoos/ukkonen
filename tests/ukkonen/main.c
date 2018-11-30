#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../src/ukkonen/ukkonen.h"
#include "../util.h"

// The amount of outgoing edges from root must be equal to the amount of unique characters in the string
void test_root_outgoing_edges(node* root, const char* string) {

    int amount   = unique_chars(string);
    int c_amount = 0;

    for (int i = 0; i < ASCII_LENGTH; i++) {

        edge* c_edge = root->outgoing_edges[i];

        if (c_edge != 0)
            c_amount++;

    }

    printf("* Test result of outgoing edges from root:\n");
    printf("  - Unique characters = %d \n", amount);
    printf("  - Outgoing edges    = %d \n", c_amount);

    if (amount == c_amount)
        printf("  => Test passed.\n");
    else
        printf("  => Test failed.\n");
}

// Helper function for counting the leaf nodes
void count_leaf_nodes_helper(node *n, int *counter) {

    if (!n->outgoing_edges)
        (*counter)++;
    else {
        for (int i = 0; i < ASCII_LENGTH; i++) {
            edge* e = n->outgoing_edges[i];
            if (e != 0)
                count_leaf_nodes_helper(e->end_node, counter);
        }
    }
}

// Counts the leaf nodes of a given suffix tree
int count_leaf_nodes(node* root) {

    int* counter = malloc(sizeof(int));
    *counter = 0;
    count_leaf_nodes_helper(root, counter);

    return *counter;
}


// The amount of leaf nodes must be equals to the length of the string
void test_leaf_nodes(node* root, const char* string) {

    int leaf_nodes = count_leaf_nodes(root);
    int characters = (int) strlen(string);

    printf("* Test result of leaf nodes:\n");
    printf("  - Amount of leaf nodes = %d \n", leaf_nodes);
    printf("  - Length of the string = %d \n", characters);

    if (leaf_nodes == characters)
        printf("  => Test passed.\n");
    else
        printf("  => Test failed.\n");
}

// Traverse a node in a DFS way
void traverse_node(node *n, int* suffix_check, int from, int prev_depth, int curr_depth) {

    if (n->outgoing_edges) {

        for (int i = 0; i < ASCII_LENGTH; i++) {
            edge* e = n->outgoing_edges[i];

            if (e != 0) {
                traverse_node(e->end_node, suffix_check, e->from, curr_depth,
                           curr_depth + (*e->to - e->from) + 1);
            }
        }

    } else {
        // When we end at a leaf node we use the given information to obtain which substring it represents
        // Afterwards, we set the value of this substring in our suffix_check to 1
        suffix_check[from - prev_depth] = 1;
    }
}

// Test if all suffixes are in the tree
void test_suffixes(node* root, const char* string) {

    size_t length       = strlen(string);
    // This array of ints is used to check if every suffix is in the suffix tree
    int*   suffix_check = calloc(length, sizeof(int));

    traverse_node(root, suffix_check, 0, 0, 0);

    printf("* Test result of suffixes:\n");

    for (int i = 0; i < length; i++) {
        if (suffix_check[i] == 0) {
            printf("  - Suffix starting at index %d is not in the tree.\n", i);
            printf("  => Test failed.\n");
            return;
        }

    }

    printf("  - All suffixes are in the tree.\n");
    printf("  => Test passed.\n");

}

// Run all tests on a given string
void test(char* file_name) {

    printf("Testing filename: %s \n\n", file_name);

    int*         ep = malloc(sizeof(int));
    FILE*        fp = fopen(file_name, "r");
    suffix_tree* sf = create_suffix_tree_from_stream(fp, ep);

    test_root_outgoing_edges(sf->root, sf->string);
    test_leaf_nodes(sf->root, sf->string);
    test_suffixes(sf->root, sf->string);

    printf("\n");

    free_node(sf->root, ep);
    free(ep);

    fclose(fp);
}

int main(int argc, char* argv[]) {

    printf("--------------------\n");
    printf("TEST PROGRAM UKKONEN\n");
    printf("--------------------\n\n");
    printf("Disclaimer: make sure that each file ends\n");
    printf("with a unique character or these tests won't work.\n");

    for (int i = 1; i < argc; i++) {
        test(argv[i]);
    }

    return 0;
}