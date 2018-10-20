#include <stdio.h>
#include <stdlib.h>
#include "ukkonen.h"

char* read_file() {

    int   c_size = 64;
    char* buffer = malloc(sizeof(char) * c_size);
    int   length = 0;
    char c;

    while ((c = (char) fgetc(stdin)) != EOF) {

        if (length >= c_size) {
            buffer = realloc(buffer, sizeof(char) * (c_size *= 2));
        }
        buffer[length++] = c;
    }

    printf("Read: %s \n", buffer);

    return buffer;
}

int main() {

    char* string = read_file();
    suffix_tree* st = create_suffix_tree(string);
    print_suffix_tree(st);
    // TODO: free suffix_tree if time over
    free(string);
    return 0;
}