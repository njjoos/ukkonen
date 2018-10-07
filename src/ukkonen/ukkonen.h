#ifndef SRC_UKKONEN_H
#define SRC_UKKONEN_H

struct edge;

typedef struct node {
    struct edge** outgoing_edges;
} node;

typedef struct edge {
    struct node* end_node;
    int*         from;
    int*         to;
} edge;

typedef struct active_point {
    struct node* active_node;
    char         active_edge;
    int          active_length;
} active_point;

typedef struct suffix_tree {
    struct node* root;
    char*        string;
} suffix_tree;

node*         create_node();
edge*         create_edge(int*, int*);
suffix_tree*  create_suffix_tree(char*);
void          print_node(node*, int);
void          print_suffix_tree(suffix_tree*);

#endif //SRC_UKKONEN_H
