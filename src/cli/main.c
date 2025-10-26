#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "repl.h"

int main(int argc, char* argv[]) {

    const char *err_string = repl_init(); // Initialize CLI interface

    if (err_string) {
        fprintf(stderr, "Fatal Error: %s\n", err_string);
        repl_shutdown(); // Ensure cleanup
        return EXIT_FAILURE;
    }

    while(repl_read_eval_print() != 0)
        ;
    printf("Exiting Expresso.\n");

    repl_shutdown(); // Clean up CLI interface
    return 0;
}
