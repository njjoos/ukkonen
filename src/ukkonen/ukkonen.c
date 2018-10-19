#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ukkonen.h"

#define ASCII_LENGTH 256

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

active_point* create_and_init_active_point(node* root) {

    active_point* ap  = malloc(sizeof(active_point));
    ap->active_node   = root;
    ap->active_edge   = '\0';
    ap->active_length = 0;

    return ap;
}

edge* create_edge(int from, int* to) {

    edge* e     = malloc(sizeof(edge));
    e->end_node = create_leaf_node();
    e->from     = from;
    e->to       = to;
}

node* split_edge(active_point* ap, const char* string, char cc, int from, int* to) {

    edge* current_edge = ap->active_node->outgoing_edges[ap->active_edge];
    node* node1 = current_edge->end_node;
    node* node2 = create_internal_node();

    // Create new leaf node for new character
    node2->outgoing_edges[cc] = create_edge(from, to);

    // Fix the rest
    int  ex_from = current_edge->from + ap->active_length;
    int* ex_to = current_edge->to;
    node2->outgoing_edges[string[ex_from]] = create_edge(ex_from, ex_to);
    node2->outgoing_edges[string[ex_from]]->end_node = node1;

    current_edge->to = malloc(sizeof(int));
    *current_edge->to = current_edge->from + ap->active_length - 1;
    current_edge->end_node = node2;

    return node2;
}

void fix_active_point(active_point *ap, const char *string, int current_index) {

    edge* current_edge = ap->active_node->outgoing_edges[ap->active_edge];
    if (current_edge) {
        int edge_length = *current_edge->to - current_edge->from + 1;

        if (ap->active_length == edge_length && current_edge->end_node) {
            ap->active_node = current_edge->end_node;
            ap->active_edge = '\0';
            ap->active_length = 0;
        } else if (ap->active_length > edge_length) {
            ap->active_node = current_edge->end_node;
            ap->active_edge = string[current_index + 1];
            ap->active_length -= edge_length;
            // It might be that the length of the next active_edge is also too small so recall it
            fix_active_point(ap, string, current_index);
        }
    }

}

suffix_tree* create_suffix_tree(char* string) {

    // TODO: fully test algorithm [easy-medium] (how? best way? ..?)

    int*          end_point  = malloc(sizeof(int));
    int           length     = (int) strlen(string);
    node*         root       = create_internal_node();
    active_point* ap         = create_and_init_active_point(root);
    int           remainder  = 0;

    root->id = 0;

    for (int i = 0; i < length; i++) {
        *end_point = i;
        remainder++;
        node* previous_node = NULL;
        char cc = string[i]; // cc = current character

        while (remainder > 0) {

            if (ap->active_length == 0) {

                // When the active length is zero, we just add a leaf node to the active node
                if (ap->active_node->outgoing_edges[cc] != 0) {
                    // If there exists an edge with the current character, update the active point
                    ap->active_edge = cc;
                    ap->active_length++;
                    // We're done with the current character, go to the next
                    goto next;
                } else {
                    // There is no edge, create one with the current character
                    ap->active_node->outgoing_edges[cc] = create_edge(i, end_point);
                    remainder--;

                    // Rule 3
                    if (ap->active_node != root) {
                        if (ap->active_node->suffix_link == NULL)
                            ap->active_node = root;
                        else
                            ap->active_node = ap->active_node->suffix_link;
                    }
                }
            } else {
                // Active length > 0: we end at the middle of an edge
                edge* current_edge = ap->active_node->outgoing_edges[ap->active_edge];
                char next_char = string[current_edge->from + ap->active_length];

                if (cc == next_char) {
                    // Current character exists on edge behind the active point
                    ap->active_length++;
                    // Check if we reached the end of our edge which then the active point must be changed
                    fix_active_point(ap, string, i-remainder+1);
                    // We're done with the current character, go to the next one
                    goto next;
                } else {
                    // New character on current edge => split
                    node* new_node = split_edge(ap, string, cc, i, end_point);

                    // Rule 2: add suffix links
                    if (previous_node != NULL) {
                        previous_node->suffix_link = new_node;
                        previous_node = new_node;
                    } else
                        previous_node = new_node;

                    if (ap->active_node != root) {
                        // Rule 3
                        if (ap->active_node->suffix_link == NULL)
                            ap->active_node = root;
                        else
                            ap->active_node = ap->active_node->suffix_link;
                    } else {
                        // Rule 1
                        ap->active_length--;
                        ap->active_edge = string[i - ap->active_length];
                        fix_active_point(ap, string, i-remainder+2);
                    }

                    remainder--;
                }
            }
        }
        // Do nothing, used to get out of while loop
        next: {};
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

void print_node(node* n, int from, int to, int prev_depth, int curr_depth) {

    if (n->outgoing_edges != NULL) {
        // Internal node
        char str[256];
        if (curr_depth == 0)
            sprintf(str, "%d @  -  = ", n->id);
        else
            sprintf(str, "%d @ %d-%d = ", n->id, from - prev_depth, to);
        for (int i = 0; i < ASCII_LENGTH; i++) {
            edge* e = n->outgoing_edges[i];

            if (e != 0) {
                char str_e[128];
                sprintf(str_e, " %c:%d,%d-%d |", i, e->end_node->id, e->from, *e->to);
                strcat(str, str_e);
            }
        }
        str[strlen(str)-1] = ' ';
        strcat(str, "\n");

        printf(str);

        for (int i = 0; i < ASCII_LENGTH; i++) {
            edge* e = n->outgoing_edges[i];

            if (e != 0) {
                print_node(e->end_node, e->from, *e->to, curr_depth, curr_depth + (*e->to - e->from) + 1);
            }
        }
    } else {
        // Leaf node
        printf("%d @ %d-%d\n", n->id, from - prev_depth, to);
    }
}

void print_suffix_tree(suffix_tree* tree) {

    int* id = malloc(sizeof(int));
    *id = 0;
    apply_ids(tree->root, id);
    print_node(tree->root, 0 , 0, 0, 0);
    free(id);
}
