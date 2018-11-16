#include <stdio.h>
#include <stdlib.h>
#include "ukkonen.h"

char* read_file() {

    int   c_size = 128;
    char* buffer = malloc(sizeof(char) * c_size);
    int   length = 0;
    char c;

    while ((c = (char) fgetc(stdin)) != EOF) {

        if (length >= c_size) {
            buffer = realloc(buffer, sizeof(char) * (c_size *= 2));
        }

        buffer[length++] = c;
    }

    fclose(stdin);
    return buffer;
}

int main() {

    char* s         = read_file();
    int*  end_point = malloc(sizeof(int));
    node* root      = create_suffix_tree(s, end_point);

    print_suffix_tree(root);
    free_node(root, end_point);
    free(end_point);
    free(s);

    return 0;
}