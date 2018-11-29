#include <stdio.h>
#include <stdlib.h>
#include "lz77.h"
#include "../ukkonen/ukkonen.h"

#define SIZE 9
//#define TEST

void check_edge_length(match_state* ms) {
    edge* current_edge = ms->a_node->outgoing_edges[ms->a_edge];
    int   edge_length  = *current_edge->to - current_edge->from + 1;

    if (ms->a_len == edge_length) {
        ms->a_node = current_edge->end_node;
        ms->a_len  = 0;
    }
}

void find_match(const char* string, char c, match_state* ms) {

    // TODO: make unsigned in array subscript
    if (ms->a_len == 0) {
        // We end at a node
        if (ms->a_node->outgoing_edges && ms->a_node->outgoing_edges[c]) {
            // There is an edge starting with the current character
            // We have a match so we update the match state
            ms->a_len  = 1;
            ms->a_edge = c;
            ms->pos    = (uint32_t) ms->a_node->outgoing_edges[c]->end_node->beg;
            ms->len++;
            // TODO fix possible end at edge
            check_edge_length(ms);
        } else {
            // The is no edge so we have no match
            ms->state = 1;
        }
    } else {
        // We end at the middle of an edge
        edge* curr_edge = ms->a_node->outgoing_edges[ms->a_edge];
        char  next_char = string[curr_edge->from + ms->a_len];

        if (next_char == c) {
            // The next character is the same as our current one
            // We have a match so we update the match state
            ms->a_len++;
            ms->len++;
            // TODO fix possible end at edge
            check_edge_length(ms);
        } else {
            // The next character is not the same, we have no match
            ms->state = 1;
        }
    }

}

void reset_match_state(match_state* ms, node* root) {
    ms->a_node = root;
    ms->a_edge = '\0';
    ms->a_len  = 0;
    ms->pos    = 0;
    ms->len    = 0;
    ms->state  = 0;
}

void compress(FILE* stream) {

    // String allocation
    int   c_size = 256;
    char* buffer = malloc(sizeof(char) * c_size);
    int   length = 0;
    char  c;

    // Tree allocation
    node*         root       = create_internal_node();
    active_point* ap         = create_and_init_active_point(root);
    int           remainder  = 0;
    int           end_point  = 0;
    int           split_c    = 0;

    root->id   = 0;
    root->beg  = 0;

    match_state ms;
    reset_match_state(&ms, root);

    while ((c = (char) fgetc(stream)) != EOF) {

        if (length == c_size)
            buffer = realloc(buffer, sizeof(char) * (c_size *= 2));

        buffer[length] = c;

        find_match(buffer, c, &ms);

        add_to_suffix_tree(root, ap, &remainder, buffer, &end_point, length);

        if (ms.state) {
            // There was no match, we print
            output o = {ms.pos, ms.len, (uint8_t) c};
            fwrite(&o, SIZE, 1, stdout);

            reset_match_state(&ms, root);
        }

        length++;

    }

    find_match(buffer, '\0', &ms);
    output o = {ms.pos, ms.len, (uint8_t) '\0'};
    fwrite(&o, SIZE, 1, stdout);

    free(ap);

}



void decompress(FILE* stream) {

    int   c_size = 256;
    char* buffer = malloc(sizeof(char) * c_size);
    int   length = 0;

    output o;

    while (fread(&o, SIZE, 1, stream)) {

        if (length == c_size)
            buffer = realloc(buffer, sizeof(char) * (c_size *= 2));

        for (int i = o.p; i < o.p + o.l; i++) {
            if (length == c_size)
                buffer = realloc(buffer, sizeof(char) * (c_size *= 2));

            buffer[length++] = buffer[i];
        }

        buffer[length++] = o.c;
    }

    printf(buffer);

}