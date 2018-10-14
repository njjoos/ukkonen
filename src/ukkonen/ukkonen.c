#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ukkonen.h"

#define ASCII_LENGTH 128

node* create_leaf_node() {

    node* n           = malloc(sizeof(node));
    n->outgoing_edges = NULL;
    n->suffix_link    = NULL;

    return n;
}

node* create_internal_node() {

    node* n           = malloc(sizeof(node));
    n->outgoing_edges = calloc(ASCII_LENGTH, sizeof(edge*)); // calloc to set everything to 0
    n->suffix_link    = NULL;

    return n;
}

edge* create_edge(int from, int* to) {

    edge* e     = malloc(sizeof(edge));
    e->end_node = create_leaf_node();
    e->from     = from;
    e->to       = to;
}

suffix_tree* create_suffix_tree(char* string) {

    int*         end_point  = malloc(sizeof(int));
    int          length     = (int) strlen(string);
    node*        root       = create_internal_node();
    active_point ap         = {root, '\0', 0};
    int          remainder  = 0;

    root->id = 0;

    for (int i = 0; i <= length; i++) {
        *end_point = i;
        remainder++;
        // node* previous_node = NULL; TODO: suffix_link addition
        char cc = string[i]; // cc = current character

        while (remainder > 0) {
            if (ap.active_length == 0) {
                // When the active length is zero, we just add a leaf node to root.
                if (root->outgoing_edges[cc] != 0) {
                    // If there exists an edge with the current character, update the active point
                    ap.active_edge = cc;
                    ap.active_length++;
                } else {
                    // There is no edge, create one with the current character
                    root->outgoing_edges[cc] = create_edge(i, end_point);
                    remainder--;
                }
            } else {
                // TODO
            }
        }
    }

    suffix_tree* st  = malloc(sizeof(suffix_tree));
    st->root   = root;
    st->string = string;

    return st;
}

void apply_ids(node* n, int* id) {

    // Depth first application of all the ids
    for (int i = 0; i < ASCII_LENGTH; i++) {
        edge* e = n->outgoing_edges[i];
        if (e != 0) {
            e->end_node->id = ++(*id);
            if (e->end_node->outgoing_edges != NULL) {
                apply_ids(e->end_node, id);
            }
        }
    }
}

void print_node(node* n, int from, int to) {

    if (n->outgoing_edges != NULL) {
        // Internal node
        printf("TODO: internal node! \n");
        for (int i = 0; i < ASCII_LENGTH; i++) {
            edge* e = n->outgoing_edges[i];
            if (e != 0) {
                print_node(e->end_node, e->from, *e->to);
            }
        }
    } else {
        // Leaf node
        printf("%d @ %d-%d\n", n->id, from, to);
    }
}

void print_suffix_tree(suffix_tree* tree) {

    printf("\nString: %s\n\n", tree->string);
    int* id = malloc(sizeof(int));
    *id = 0;
    apply_ids(tree->root, id);
    print_node(tree->root, 0 , 0);
    printf("Gank: %d\n", *id);
    free(id);
}


