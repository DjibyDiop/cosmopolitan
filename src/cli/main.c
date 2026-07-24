#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uex.h"

extern int uex_inspect(const char *filename);
extern int uex_pack(int argc, char **argv);

void print_usage(const char *prog) {
    printf("UEX (Universal Executable Extension) CLI\n");
    printf("Usage: %s <command> [args...]\n\n", prog);
    printf("Commands:\n");
    printf("  inspect <file.uex>        Inspect a UEX binary and its chunks\n");
    printf("  pack --manifest <file>    Pack a new UEX binary\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "inspect") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: missing filename for inspect\n");
            return 1;
        }
        return uex_inspect(argv[2]);
    } else if (strcmp(argv[1], "pack") == 0) {
        return uex_pack(argc - 1, argv + 1);
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
