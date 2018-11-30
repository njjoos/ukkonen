#include <stdio.h>
#include <stdlib.h>
#include "lz77.h"
#include "../ukkonen/ukkonen.h"

#define SIZE 9
#define MAX_LENGTH 1000000
//#define TEST

// Checks if we reached the end of an edge and updates the status
void check_edge_length(match_state* ms) {

    edge* current_edge = ms->a_node->outgoing_edges[(unsigned) ms->a_edge];
    int   edge_length  = *current_edge->to - current_edge->from + 1;

    if (ms->a_len == edge_length) {
        ms->a_node = current_edge->end_node;
        ms->a_len  = 0;
    }
}

// Checks the next character in the suffix tree to retrieve a match
void find_match(const char* string, char c, match_state* ms) {

    if (ms->a_len == 0) {
        // We end at a node
        if (ms->a_node->outgoing_edges && ms->a_node->outgoing_edges[(unsigned) c]) {
            // There is an edge starting with the current character
            // We have a match so we update the match state
            ms->a_len  = 1;
            ms->a_edge = c;
            ms->pos    = (uint32_t) ms->a_node->outgoing_edges[(unsigned) c]->end_node->beg;
            ms->len++;

            check_edge_length(ms);
        } else {
            // The is no edge so we have no match
            ms->state = 1;
        }
    } else {
        // We end at the middle of an edge
        edge* curr_edge = ms->a_node->outgoing_edges[(unsigned) ms->a_edge];
        char  next_char = string[curr_edge->from + ms->a_len];

        if (next_char == c) {
            // The next character is the same as our current one
            // We have a match so we update the match state
            ms->a_len++;
            ms->len++;

            check_edge_length(ms);
        } else {
            // The next character is not the same, we have no match
            ms->state = 1;
        }
    }
}

// Resets the match state
void reset_match_state(match_state* ms, node* root) {

    ms->a_node = root;
    ms->a_edge = '\0';
    ms->a_len  = 0;
    ms->pos    = 0;
    ms->len    = 0;
    ms->state  = 0;
}



// Compresses the contents of a file stream
void compress(FILE* stream, int optimized) {

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

    root->id   = 0;
    root->beg  = 0;

    // Match state
    match_state ms;
    reset_match_state(&ms, root);

    while ((c = (char) fgetc(stream)) != EOF) {

        // Optimized memory usage
        if (optimized && length >= MAX_LENGTH) {

            // First print out what we currently have with a unique character at the end
            // (uint8_t) ~0 = 255
            output o = {ms.pos, ms.len, (uint8_t) ~0};
            #ifndef TEST
            fwrite(&o, SIZE, 1, stdout);
            #else
            printf("%d %d [%d] \n", o.p, o.l, o.c);
            #endif

            // Free
            free(buffer);
            free_node(root, &end_point);
            free(ap);

            // Reset everything
            c_size = 256;
            buffer = malloc(sizeof(char) * c_size);
            length = 0;

            root       = create_internal_node();
            ap         = create_and_init_active_point(root);
            remainder  = 0;
            end_point  = 0;

            root->id   = 0;
            root->beg  = 0;

            reset_match_state(&ms, root);
        }

        if (length == c_size)
            buffer = realloc(buffer, sizeof(char) * (c_size *= 2));

        buffer[length] = c;

        find_match(buffer, c, &ms);

        add_to_suffix_tree(root, ap, &remainder, buffer, &end_point, length);

        if (ms.state) {
            // There was no match, we print
            output o = {ms.pos, ms.len, (uint8_t) c};
            #ifndef TEST
            fwrite(&o, SIZE, 1, stdout);
            #else
            printf("%d %d %c \n", o.p, o.l, o.c);
            #endif
            reset_match_state(&ms, root);
        }

        length++;

    }

    find_match(buffer, '\0', &ms);

    output o = {ms.pos, ms.len, (uint8_t) '\0'};
    #ifndef TEST
    fwrite(&o, SIZE, 1, stdout);
    #else
    printf("%d %d [%d] \n", o.p, o.l, o.c);
    #endif
    free(ap);
    free_node(root, &end_point);
    free(buffer);
}

// Decompresses
void decompress(FILE* stream, int optimized) {

    int   c_size = 256;
    char* buffer = malloc(sizeof(char) * c_size);
    int   length = 0;

    output o;

    while (fread(&o, SIZE, 1, stream)) {

        // It might be that o.l is way bigger than c_size (e.g. o.l = 10000, c_size = 256)
        // but this will never because o.l is always smaller than c_size as o.l is the length of a
        // given position in the buffer that has size c_size
        if (length + o.l >= c_size) {
            c_size *= 2;
            buffer = realloc(buffer, sizeof(char) * c_size);
        }

        for (int i = o.p; i < o.p + o.l; i++) {

            buffer[length++] = buffer[i];
        }

        if (optimized && o.c == (uint8_t) ~0) {
            fwrite(buffer, sizeof(char), (size_t) length, stdout);
            free(buffer);

            c_size = 256;
            buffer = malloc(sizeof(char) * c_size);
            length = 0;
        } else
            buffer[length++] = (char) o.c;
    }

    fwrite(buffer, sizeof(char), (size_t) length-1, stdout);

}