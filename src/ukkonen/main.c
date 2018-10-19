#include <stdio.h>
#include <stdlib.h>
#include "ukkonen.h"

int main() {

    suffix_tree* st = create_suffix_tree("maandag zag ik haar$");
    print_suffix_tree(st);
    // TODO: free suffix_tree if time over
    return 0;
}