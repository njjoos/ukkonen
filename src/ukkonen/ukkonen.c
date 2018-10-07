#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ukkonen.h"

#define ASCII_LENGTH 128

node* create_node() {

    node* n = malloc(sizeof(node));
    n->outgoing_edges = malloc(ASCII_LENGTH * sizeof(edge*));
    return n;
}

suffix_tree* create_suffix_tree(char* string) {

    int*  current    = malloc(sizeof(int));
    int   length     = (int) strlen(string);
    node* root       = create_node();
    active_point ap  = {root, '\0', 0};
    suffix_tree* st  = malloc(sizeof(suffix_tree));

    *current   = 0;
    st->root   = root;
    st->string = string;

    return st;
}

void print_suffix_tree(suffix_tree* tree) {
    printf("%s\n", tree->string);
}
