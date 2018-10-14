#include <stdio.h>
#include <stdlib.h>
#include "ukkonen.h"

int main() {

    suffix_tree* st = create_suffix_tree("abc");
    print_suffix_tree(st);

    return 0;
}