#include <stdio.h>
#include <string.h>
#include "lz77.h"

int main(int argc, char** argv) {

    int c = 0;
    int d = 0;
    int o = 0;

    if (argc == 1) {
        printf("Error: no flag was given.\n");
    }

    for (int i = 1; i < argc; i++) {
        if (!strcmp("-o", argv[i]))
            o = 1;
        else if (!strcmp("-c", argv[i])) {
            if (d == 1) {
                printf("Error: can't compress and decompress at the same time.\n");
                return 1;
            } else
                c = 1;
        } else if (!strcmp("-d", argv[i])) {
            if (c == 1) {
                printf("Error: can't compress and decompress at the same time.\n");
                return 1;
            } else
                d = 1;
        } else {
            printf("Error: %s is not a legal flag.\n", argv[i]);
            return 2;
        }
    }

    if (c)
        compress(stdin, stdout, o);

    if (d)
        decompress(stdin, stdout, o);

    return 0;
}