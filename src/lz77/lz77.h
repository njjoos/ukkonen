#ifndef SRC_LZ77_H
#define SRC_LZ77_H

#include <stdint.h>

struct node;

typedef struct output {
    uint32_t p;
    uint32_t l;
    uint8_t  c;
} output;

typedef struct match_state {
    struct node* a_node;
    char         a_edge;
    int          a_len;
    uint32_t     pos;
    uint32_t     len;
    int          state; // 1: No match | 0: Match
} match_state;

void compress(FILE*);
void decompress(FILE*);

#endif //SRC_LZ77_H
