#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ukkonen.h"

// Creates a leaf node
node* create_leaf_node() {

    node* n           = malloc(sizeof(node));
    n->outgoing_edges = NULL;
    n->suffix_link    = NULL;

    return n;
}

// Creates an internal node, the only difference is that this has outgoing edges
node* create_internal_node() {

    node* n           = malloc(sizeof(node));
    n->outgoing_edges = calloc(ASCII_LENGTH, sizeof(edge*)); // calloc to set everything to 0
    n->suffix_link    = NULL;

    return n;
}

// Creates and initializes the active point that will be used
active_point* create_and_init_active_point(node* root) {

    active_point* ap  = malloc(sizeof(active_point));
    ap->active_node   = root;
    ap->active_edge   = 0;
    ap->active_length = 0;

    return ap;
}

// Creates an edge
edge* create_edge(int from, int* to, int beg) {

    edge* e        = malloc(sizeof(edge));
    node* end_node = create_leaf_node();
    end_node->beg  = beg;
    e->end_node    = end_node;
    e->from        = from;
    e->to          = to;

    return e;
}

// Splits an edge based on the active point
node* split_edge(active_point* ap, const char* string, char cc, int from, int* to, int* remainder) {

    edge* current_edge = ap->active_node->outgoing_edges[(unsigned) string[ap->active_edge]];
    node* node1        = current_edge->end_node;
    node* node2        = create_internal_node();

    // Create new edge for new character
    node2->outgoing_edges[(unsigned) cc] = create_edge(from, to, from - *remainder + 1);

    // Fix latter part of previous edge
    int   ex_from = current_edge->from + ap->active_length;
    int*  ex_to   = current_edge->to;
    edge* e_edge  = malloc(sizeof(edge));

    e_edge->from     = ex_from;
    e_edge->to       = ex_to;
    e_edge->end_node = node1;

    node2->outgoing_edges[(unsigned) string[ex_from]] = e_edge;
    node2->beg = node1->beg;

    // Fix prior part of previous edge
    current_edge->to       = malloc(sizeof(int));
    *current_edge->to      = current_edge->from + ap->active_length - 1;
    current_edge->end_node = node2;

    // Finally return the new internal node
    return node2;
}

// Traverses down the tree to avoid active lengths that are bigger than the length of the active edge
void traverse_down(active_point* ap, const char* string) {

    edge* current_edge = ap->active_node->outgoing_edges[(unsigned) string[ap->active_edge]];
    int   edge_length  = *current_edge->to - current_edge->from + 1;

    if (ap->active_length >= edge_length) {
        ap->active_node   =  current_edge->end_node;
        ap->active_length -= edge_length;
        ap->active_edge   += edge_length;

        // It might be that the active length is still too large, call this function again
        if (ap->active_length > 0)
            traverse_down(ap, string);
    }
}

void add_to_suffix_tree(node* root, active_point* ap, int* remainder, char* curr_string,
                        int* end_point, int curr_length) {

    *end_point          = curr_length;
    node* previous_node = NULL;
    char  cc            = curr_string[curr_length]; // cc = current character
    (*remainder)++;

    while (*remainder > 0) {

        if (ap->active_length == 0) {
            // When the active length is 0, we are adding from the root
            if (root->outgoing_edges[(unsigned) cc] != 0) {
                // If there exists an edge with the current character, update the active point
                ap->active_edge   = curr_length;
                ap->active_length = 1;
                // We're done with the current character, go to the next
                break;
            } else {
                // There is no edge, create one with the current character
                (*remainder)--;
                root->outgoing_edges[(unsigned) cc] = create_edge(curr_length, end_point, curr_length - *remainder);

            }
        } else {
            // Active length > 0: we end at the middle of an edge
            // Traverse down the tree, this is needed when active length >= current edge length
            traverse_down(ap, curr_string);

            if (ap->active_length == 0) {
                // When the active length is 0 after traversing, we practically do the same as with root
                // except that we need to apply Rule 4 when we create a new leaf node
                if (ap->active_node->outgoing_edges[(unsigned) cc] != 0) {
                    // If there exists an edge with the current character, update the active point
                    ap->active_edge   = curr_length;
                    ap->active_length = 1;
                    // We're done with the current character, go to the next
                    break;
                } else {
                    // There is no edge, create one with the current character
                    (*remainder)--;
                    ap->active_node->outgoing_edges[(unsigned) cc] = create_edge(curr_length, end_point,
                            curr_length - *remainder);

                    // Rule 4
                    ap->active_node   = root;
                    ap->active_length = *remainder - 1;
                    ap->active_edge   = curr_length - *remainder + 1;
                }
            } else {
                // When the active length > 0, we end at the middle of an edge
                edge* current_edge = ap->active_node->outgoing_edges[(unsigned) curr_string[ap->active_edge]];
                char  next_char    = curr_string[current_edge->from + ap->active_length];

                if (cc == next_char) {
                    // Update active point
                    ap->active_length++;
                    break;
                } else {
                    // Split edge
                    node* new_node = split_edge(ap, curr_string, cc, curr_length, end_point, remainder);
                    (*remainder)--;

                    // Rule 2
                    if (previous_node)
                        previous_node->suffix_link = new_node;
                    previous_node = new_node;

                    if (ap->active_node == root) {
                        // Rule 1
                        ap->active_length--;
                        ap->active_edge++;
                    } else {
                        // Rule 3
                        if (ap->active_node->suffix_link)
                            ap->active_node = ap->active_node->suffix_link;
                        else {
                            // Rule 4
                            ap->active_node   = root;
                            ap->active_length = *remainder - 1;
                            ap->active_edge   = curr_length - *remainder + 1;
                        }
                    }
                }
            }
        }
    }
}

// Create a suffix tree using Ukkonen's algorithm
suffix_tree* create_suffix_tree_from_stream(FILE *stream, int *end_point) {

    // String allocation
    int   c_size = 256;
    char* buffer = malloc(sizeof(char) * c_size);
    int   length = 0;
    char  c;

    // Tree allocation
    node*         root       = create_internal_node();
    active_point* ap         = create_and_init_active_point(root);
    int*          remainder  = malloc(sizeof(int));

    *remainder = 0;
    root->id   = 0;
    root->beg  = 0;

    while ((c = (char) fgetc(stream)) != EOF) {

        if (length == c_size)
            buffer = realloc(buffer, sizeof(char) * (c_size *= 2));

        buffer[length] = c;

        add_to_suffix_tree(root, ap, remainder, buffer, end_point, length++);
    }

    free(ap);
    free(remainder);

    suffix_tree* sf = malloc(sizeof(suffix_tree));

    sf->root   = root;
    sf->string = buffer;

    return sf;
}

// Print a node in a DFS way
void print_node(node *n, int *id, int from, int to, int prev_depth, int curr_depth) {

    if (n->outgoing_edges) {
        // Internal node
        for (int i = 0; i < ASCII_LENGTH; i++) {
            edge* e = n->outgoing_edges[i];

            if (e != 0) {
                e->end_node->id = ++(*id);
                print_node(e->end_node, id, e->from, *e->to, curr_depth, curr_depth + (*e->to - e->from) + 1);
            }
        }

        if (curr_depth == 0)
            // Root
            printf("[%d] %d @  -  = ", n->beg, n->id);
        else
            // Other
            printf("[%d] %d @ %d-%d = ", n->beg, n->id, from - prev_depth, to);

        int children = 0;

        for (int i = 0; i < ASCII_LENGTH; i++) {

            edge* e = n->outgoing_edges[i];

            if (e != 0) {

                if (children++)
                    printf("| %d:%d,%d-%d ", i, e->end_node->id, e->from, *e->to);
                else
                    printf("%d:%d,%d-%d ", i, e->end_node->id, e->from, *e->to);
            }
        }

        printf("\n");

    } else {
        // Leaf node
        printf("[%d] %d @ %d-%d\n",n->beg, n->id, from - prev_depth, to);
    }
}

// Main function for printing a suffix tree
void print_suffix_tree(node* root) {

    int* id = malloc(sizeof(int));
    *id     = 0;

    print_node(root, id, 0, 0, 0, 0);
    free(id);
}

// Free a given node
void free_node(node* n, int* end_point) {

    if (n->outgoing_edges) {
        // Internal node
        for (int i = 0; i < ASCII_LENGTH; i++) {
            edge* e = n->outgoing_edges[i];
            if (e != 0) {
                // First free the node at the end of the edge
                free_node(e->end_node, end_point);
                if (e->to != end_point)
                    // Free end point of current edge
                    free(e->to);
                // Free edge
                free(e);
            }
        }

        // Free the outgoing edges
        free(n->outgoing_edges);
    }

    // Free node
    free(n);
}
