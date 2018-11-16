#include <stdio.h>
#include <stdlib.h>
#include "ukkonen.h"

int main() {

    int* end_point = malloc(sizeof(int));
    *end_point = 0;

    node* root = create_suffix_tree_from_stream(stdin, end_point);
    print_suffix_tree(root);

    free_node(root, end_point);
    free(end_point);

    return 0;
}