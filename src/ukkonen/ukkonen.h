#ifndef SRC_UKKONEN_H
#define SRC_UKKONEN_H

#define ASCII_LENGTH 128

struct edge;

typedef struct node {
    struct edge** outgoing_edges;
    struct node*  suffix_link;
    int           id;
    int           beg;
} node;

typedef struct edge {
    struct node* end_node;
    int          from;
    int*         to;
} edge;

typedef struct active_point {
    struct node* active_node;
    int          active_edge;
    int          active_length;
} active_point;

typedef struct suffix_tree {
    struct node* root;
    char*        string;
} suffix_tree;


node*         create_internal_node();
active_point* create_and_init_active_point(node*);
void          add_to_suffix_tree(node*, active_point*, int*, char*, int*, int);
suffix_tree*  create_suffix_tree_from_stream(FILE *, int *);
void          print_suffix_tree(node*);
void          free_node(node*, int*);

#endif //SRC_UKKONEN_H
