#include <stdio.h>
#include <stdlib.h>
#include "lz77.h"
#include "../ukkonen/ukkonen.h"

#define BLAND_CHAR  (uint8_t) ~0 // = 255
#define MAX_LENGTH  10000000
#define START_SIZE  65536 // 2^16
#define OUTPUT_SIZE 9

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
void compress(FILE* i_stream, FILE* o_stream, int optimized) {

    // String allocation
    int   c_size = START_SIZE;
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

    while ((c = (char) fgetc(i_stream)) != EOF) {

        if (length >= MAX_LENGTH) {
            if (optimized) {
                // First print out what we currently have with a unique character at the end
                // We call this unique character BLAND_CHAR which has the same value as EOF
                output o = {ms.pos, ms.len, BLAND_CHAR};
                fwrite(&o, OUTPUT_SIZE, 1, o_stream);

                // Free
                free(buffer);
                free_node(root, &end_point);
                free(ap);

                // Reset everything
                c_size = START_SIZE;
                buffer = malloc(sizeof(char) * c_size);
                length = 0;

                root       = create_internal_node();
                ap         = create_and_init_active_point(root);
                remainder  = 0;
                end_point  = 0;

                root->id   = 0;
                root->beg  = 0;

                reset_match_state(&ms, root);
            } else {
                // When the file is "too big" we say that the user has to use the -o flag
                // in order to avoid memory problems. We send this to stderr.
                fprintf(stderr, "The file is too big to use with normal compression.\n");
                fprintf(stderr, "Use the -o flag to use the memory optimized version.\n");
                exit(1);
            }
        }

        if (length == c_size)
            buffer = realloc(buffer, sizeof(char) * (c_size *= 2));

        buffer[length] = c;

        find_match(buffer, c, &ms);

        add_to_suffix_tree(root, ap, &remainder, buffer, &end_point, length);

        if (ms.state) {
            // There was no match found with the current substring, but there was
            // without the new character, so we output the previous substring and
            // reset everything.
            output o = {ms.pos, ms.len, (uint8_t) c};
            fwrite(&o, OUTPUT_SIZE, 1, o_stream);
            reset_match_state(&ms, root);
        }

        length++;

    }

    // Finally apply the find_match with the null character to fully enclose the
    // compression and get the right result.
    find_match(buffer, '\0', &ms);

    output o = {ms.pos, ms.len, (uint8_t) '\0'};
    fwrite(&o, OUTPUT_SIZE, 1, o_stream);

    // Free stuff
    free(ap);
    free_node(root, &end_point);
    free(buffer);
}

// Decompresses
void decompress(FILE* i_stream, FILE* o_stream, int optimized) {

    int   c_size = START_SIZE;
    char* buffer = malloc(sizeof(char) * c_size);
    int   length = 0;

    output o;

    while (fread(&o, OUTPUT_SIZE, 1, i_stream)) {

        // It might be that o.l is way bigger than c_size (e.g. o.l = 10000, c_size = 256)
        // but this will never because o.l is always smaller than c_size as o.l is the length of a
        // given position in the buffer that has size c_size
        if (length + o.l >= c_size)
            buffer = realloc(buffer, sizeof(char) * (c_size *= 2));

        // Just iterate through the used indices
        for (int i = o.p; i < o.p + o.l; i++)
            buffer[length++] = buffer[i];

        if (optimized && o.c == BLAND_CHAR) {
            // When we optimized memory usage, we had to enter a special character which simply had
            // the value 255, whenever we find this character it means that we had to reset everything.
            // So here we also reset everything so correspond with the compression.
            fwrite(buffer, sizeof(char), (size_t) length, o_stream);
            free(buffer);

            c_size = START_SIZE;
            buffer = malloc(sizeof(char) * c_size);
            length = 0;
        } else
            // This has to be in the else block because we don't want to output the character 255
            buffer[length++] = (char) o.c;
    }

    fwrite(buffer, sizeof(char), (size_t) length-1, o_stream);
}