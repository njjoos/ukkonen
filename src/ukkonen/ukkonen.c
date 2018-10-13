#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ukkonen.h"

#define ASCII_LENGTH 128

node* create_node() {

    node* n           = malloc(sizeof(node));
    n->outgoing_edges = calloc(ASCII_LENGTH, sizeof(edge*)); // calloc to set everything to 0
    n->suffix_link    = NULL;

    return n;
}

edge* create_edge(int* from, int* to) {

    edge* e     = malloc(sizeof(edge));
    e->end_node = NULL;
    e->from     = from;
    e->to       = to;
}

suffix_tree* create_suffix_tree(char* string) {

    int*  end_point  = malloc(sizeof(int));
    int   length     = (int) strlen(string);
    node* root       = create_node();
    active_point ap  = {root, '\0', 0};
    int remainder    = 1;

    for (int i = 0; i <= length; i++) {
        *end_point = i;
        node* last_split = NULL;

        // Check if current character exists on active_point
        char c = ap.active_edge == '\0' ? string[i] : ap.active_edge;
        edge* current_edge = ap.active_node->outgoing_edges[c];
        if (current_edge != 0 && string[*current_edge->from + ap.active_length] == c) {
            // Edge exists with current suffix
            ap.active_edge = c;
            ap.active_length++;
            remainder++;
            // TODO: if at end of edge => active_point = {edge->end_node, '\0', 0}
        } else {
            // TODO: suffix_link
            while (remainder > 0) {
                if (current_edge == 0) {
                    // Edge does not exist on active_node => create new leaf node
                    int* begin_point = malloc(sizeof(int));
                    *begin_point = i;
                    ap.active_node->outgoing_edges[c] = create_edge(begin_point, end_point);
                } else {
                    // Edge exists but new character on that edge => split

                    // TODO: Rule 2
                }
                remainder--;
            }

            remainder = 1;

        }


    }

    suffix_tree* st  = malloc(sizeof(suffix_tree));
    st->root   = root;
    st->string = string;

    return st;
}

void print_node(node* n, int* id) {

    for (int i = 0; i < ASCII_LENGTH; i++) {
        edge* e = n->outgoing_edges[i];
        if (e != 0) {
            printf("%c @ %d-%d\n", (char) i, *e->from, *e->to);
        }
    }
}

void print_suffix_tree(suffix_tree* tree) {

    printf("String: %s\n", tree->string);
    printf("Tree:\n");
    printf("-------------------------\n");
    int* id = malloc(sizeof(int));
    *id = 0;
    print_node(tree->root, id);
    free(id);
}


