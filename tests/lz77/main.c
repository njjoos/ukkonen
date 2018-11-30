#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/lz77/lz77.h"


#define TEMP_BIN "temp.bin"
#define TEMP_TXT "temp.txt"

// Applies compression
void apply_compression(FILE* i_stream, int optimized) {

    FILE* o_stream = fopen(TEMP_BIN, "wb");

    compress(i_stream, o_stream, optimized);

    fclose(o_stream);
}

// Applies decompression
void apply_decompression(int optimized) {

    FILE* i_stream = fopen(TEMP_BIN, "rb");
    FILE* o_stream = fopen(TEMP_TXT, "w");

    decompress(i_stream, o_stream, optimized);

    fclose(i_stream);
    fclose(o_stream);
}

// Runs the diff command on the original file and the decompressed file using the system function
void apply_diff(const char* file_name) {

    size_t fnf_length = strlen(file_name);
    size_t fns_length = strlen(TEMP_TXT);

    char buffer[7 + fnf_length + fns_length]; // len(diff) = 4 | 2 spaces | EOL = 7
    sprintf(buffer, "diff %s %s", file_name, TEMP_TXT);

    int status = system(buffer)/256;

    printf("  - Status code: %d \n", status);

    if (status)
        printf("  => Test failed.\n");
    else
        printf("  => Test passed.\n");
}

// Run compression, then decompression and compare with the original
void test(const char* file_name, int optimized) {

    FILE* main_fp = fopen(file_name, "r");

    apply_compression(main_fp, optimized);
    apply_decompression(optimized);
    apply_diff(file_name);

    fclose(main_fp);
}

int main(int argc, char** argv) {

    printf("-----------------\n");
    printf("TEST PROGRAM LZ77\n");
    printf("-----------------\n\n");

    for (int i = 1; i < argc; i++) {
        printf("Testing filename: %s \n\n", argv[i]);
        printf("* Test result without -o flag:\n");
        test(argv[i], 0);
        printf("* Test result with -o flag:\n");
        test(argv[i], 1);
        printf("\n");
    }

    return 0;
}

