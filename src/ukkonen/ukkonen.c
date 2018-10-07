#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ukkonen.h"

#define ASCII_LENGTH 128

node* create_node() {

    node* n           = malloc(sizeof(node));
    n->outgoing_edges = calloc(ASCII_LENGTH, sizeof(edge*)); // calloc to set everything to 0
    return n;
}

edge* create_edge(int* from, int* to) {
    edge* e     = malloc(sizeof(edge));
    e->end_node = NULL;
    e->from     = from;
    e->to       = to;
}

suffix_tree* create_suffix_tree(char* string) {

    int*  current    = malloc(sizeof(int));
    int   length     = (int) strlen(string);
    node* root       = create_node();
    active_point ap  = {root, '\0', 0};
    int remainder    = 0;
    suffix_tree* st  = malloc(sizeof(suffix_tree));

    for (int i = 0; i <= length; i++) {
        *current = i;
        int* this = malloc(sizeof(int));
        *this = i;
        root->outgoing_edges[string[i]] = create_edge(this, current);
    }

    st->root   = root;
    st->string = string;

    return st;
}

void print_node(node* n, int id) {
    for (int i = 0; i < 128; i++) {
        edge* e = n->outgoing_edges[i];
        if (e != 0) {
            printf("%c @ %d - %d\n", (char) i, *e->from, *e->to);
        }
    }
}

void print_suffix_tree(suffix_tree* tree) {
    printf("String: %s\n", tree->string);
    printf("Tree:\n");
    printf("-------------------------\n");
    print_node(tree->root, 0);

}


