#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

int is_in(const char* string, int length, char c) {

    for (int i = 0; i < length; i++) {

        if (string[i] == c) {

            return 1;
        }
    }

    return 0;
}

int unique_chars(const char* string) {
    int   c_size = 128;
    char* buffer = malloc(sizeof(char) * c_size);
    int   length = 0;
    int   i      = 0;
    char  c      = string[0];

    while (c != EOL) {

        if (!is_in(buffer, length, c)) {
            if (length >= c_size)
                buffer = realloc(buffer, sizeof(char) * (c_size *= 2));
            buffer[length++] = c;
        }

        c = string[++i];
    }

    free(buffer);

    return length;
}

