#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h" // Include history.h
#include "cli_interface.h" // Include cli_interface.h
// Remove parser_wrapper.h and evaluator.h

// Placeholder for a readline-like function
// This version will add to history, but not yet handle arrow keys
char* read_line(History* h, const char* prompt) {
    printf("%s", prompt);
	fflush(stdout);
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    read = getline(&line, &len, stdin);
    if (read == -1) {
        free(line);
        return NULL; // EOF or error
    }
    // Remove trailing newline
    if (read > 0 && line[read - 1] == '\n') {
        line[read - 1] = '\0';
    }

    // Add to history
    if (line && strlen(line) > 0) {
        history_add(h, line);
    }

    return line;
}

int main(int argc, char* argv[]) {
    cli_init(); // Initialize CLI interface

    History* h = history_create(10); // Create history with capacity 10 (FR-007)
    if (!h) {
        fprintf(stderr, "Fatal Error: Could not create history buffer.\n");
        cli_shutdown(); // Ensure cleanup
        return EXIT_FAILURE;
    }

    char* input_line;
    while ((input_line = read_line(h, "expr> ")) != NULL) {
        if (strcmp(input_line, "!quit") == 0 || strcmp(input_line, "!exit") == 0) {
            printf("Exiting Expresso.\n");
            free(input_line);
            break;
        } else if (strcmp(input_line, "!history") == 0) {
            printf("Session History:\n");
            for (size_t i = 0; i < history_size(h); ++i) {
                // FR-008: "**[ 1]:** <line text>"
                printf("**[ %zu]:** %s\n", i + 1, history_get(h, i));
            }
        } else if (strcmp(input_line, "!clear") == 0) {
            history_clear(h);
            printf("Session history cleared.\n");
        } else if (strncmp(input_line, "!n ", 3) == 0) { // Check for "!n "
            int index = atoi(input_line + 3); // Parse index
            if (index > 0 && index <= history_size(h)) {
                const char* history_entry = history_get(h, index - 1);
                if (history_entry) {
                    printf("Re-inputting: %s\n", history_entry);
                    // In a real implementation, this would re-evaluate the expression
                    // For now, just print it.
                }
            } else {
                fprintf(stderr, "Error: history index out of bounds\n");
            }
        } else {
            Value eval_result = cli_evaluate_expression(input_line);
            value_print(eval_result);
            printf("\n"); // Newline after result
            value_destroy(eval_result);
        }
        free(input_line);
    }

    history_destroy(h); // Clean up history
    cli_shutdown(); // Clean up CLI interface
    return 0;
}

