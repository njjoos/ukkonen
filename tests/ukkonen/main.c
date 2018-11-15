#include <stdio.h>
#include <memory.h>
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

    printf(" Test result of outgoing edges from root:\n");
    printf("  Unique characters = %d \n", amount);
    printf("  Outgoing edges    = %d \n", c_amount);

    if (amount == c_amount)
        printf("  Test passed.\n");
    else
        printf("  Test failed.\n");
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

    printf(" Test result of leaf nodes:\n");
    printf("  Amount of leaf nodes = %d \n", leaf_nodes);
    printf("  Length of the string = %d \n", characters);

    if (leaf_nodes == characters)
        printf("  Test passed.\n\n");
    else
        printf("  Test failed.\n\n");
}

// Run all tests on a given string
void test(char* string) {

    int*  ep   = malloc(sizeof(int));
    node* root = create_suffix_tree(string, ep);

    printf("Testing the string: %s \n\n", string);
    print_suffix_tree(root);
    printf("\n");

    test_root_outgoing_edges(root, string);
    test_leaf_nodes(root, string);

    free_node(root, ep);
    free(ep);
}

int main() {

    test("abacabc_");
    test("eatheaheearththeearthw_");

    return 0;
}