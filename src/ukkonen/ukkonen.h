#ifndef SRC_UKKONEN_H
#define SRC_UKKONEN_H

#define ASCII_LENGTH 128

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


node* create_suffix_tree(char*, int*);
void  print_suffix_tree(node*);
void  free_node(node*, int*);

#endif //SRC_UKKONEN_H
