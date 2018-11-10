#include <stdio.h>
#include <memory.h>
#include "../../src/ukkonen/ukkonen.h"
#include "../util.h"

// The amount of outgoing edges from root must be equal to the amount of unique characters in the string
int test_root_outgoing_edges(node* root, const char* string) {
    int amount   = unique_chars(string);
    int c_amount = 0;

    for (int i = 0; i < ASCII_LENGTH; i++) {

        edge* c_edge = root->outgoing_edges[i];

        if (c_edge != 0)
            c_amount++;

    }

    return amount == c_amount ? 1 : 0;
}

// The amount of leaf nodes must be equals to the length of the string
int test_leaf_nodes(node* root, const char* string) {
    // TODO
}

int main() {

    char* gg = "putainkerlkemassashonger$";
    node* root = create_suffix_tree(gg);
    test_root_outgoing_edges(root, gg);
    return 0;
}