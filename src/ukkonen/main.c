#include <stdio.h>
#include "ukkonen.h"

int main() {
    suffix_tree* st = create_suffix_tree("gank");
    print_suffix_tree(st);
    return 0;
}