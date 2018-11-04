#ifndef SRC_UKKONEN_H
#define SRC_UKKONEN_H

struct edge;

typedef struct node {
    struct edge** outgoing_edges;
    struct node*  suffix_link;
    int           id;
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

suffix_tree* create_suffix_tree(char*);
void         print_suffix_tree(suffix_tree*);

#endif //SRC_UKKONEN_H
