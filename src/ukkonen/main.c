#include <stdio.h>
#include <stdlib.h>
#include "ukkonen.h"

int main() {

    int* end_point = malloc(sizeof(int));
    *end_point     = 0;

    suffix_tree* sf = create_suffix_tree_from_stream(stdin, end_point);
    print_suffix_tree(sf->root);

    free_node(sf->root, end_point);
    free(sf->string);
    free(sf);
    free(end_point);

    return 0;
}