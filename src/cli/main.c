#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h" // Include history.h
#include "repl.h"

int main(int argc, char* argv[]) {
    repl_init(); // Initialize CLI interface

    History* h = history_create(10); // Create history with capacity 10 (FR-007)
    if (!h) {
        fprintf(stderr, "Fatal Error: Could not create history buffer.\n");
        repl_shutdown(); // Ensure cleanup
        return EXIT_FAILURE;
    }

    while(repl_read_eval_print(h) != 0)
        ;
    printf("Exiting Expresso.\n");

    history_destroy(h); // Clean up history
    repl_shutdown(); // Clean up CLI interface
    return 0;
}
