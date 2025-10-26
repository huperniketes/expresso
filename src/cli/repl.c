#include "repl.h"
#include "parser_wrapper.h" // For C++ parser interface
#include "evaluator.h"      // For evaluator
#include "value.h"          // For Value type
#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Placeholder for a readline-like function
// This version will add to history, but not yet handle arrow keys
char* read_line(History* const h, const char* prompt) {
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

// Global parser context (managed by C++ wrapper)
static ExpressoParserContext* g_parser_ctx = NULL;

void repl_init() {
    g_parser_ctx = expresso_parser_create();
    if (!g_parser_ctx) {
        fprintf(stderr, "Fatal Error: Could not initialize parser context.\n");
        exit(EXIT_FAILURE);
    }
}

void repl_shutdown() {
    if (g_parser_ctx) {
        expresso_parser_destroy(g_parser_ctx);
        g_parser_ctx = NULL;
    }
}

Value repl_evaluate_expression(const char* input_line) {
    if (!g_parser_ctx) {
        return value_create_error("CLI interface not initialized.");
    }
    if (!input_line || strlen(input_line) == 0) {
        return value_create_error("Empty input provided for evaluation.");
    }

    ExpressoParseTree* tree = expresso_parser_parse(g_parser_ctx, input_line);
    if (tree != NULL) { // No syntax errors
        Value eval_result = evaluate_expression(tree);
        expresso_tree_destroy(tree);
        return eval_result;
    } else {
        // Error already printed by parser_wrapper
        return value_create_error("Syntax error during parsing.");
    }
}

int repl_read_eval_print(History *const h) {

    char* input_line = read_line(h, "expr> ");
    int   continue_repl = input_line != NULL;

    if (continue_repl) {
        if (strcmp(input_line, "!quit") == 0 || strcmp(input_line, "!exit") == 0) {
            continue_repl = 0;
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
            Value eval_result = repl_evaluate_expression(input_line);
            value_print(eval_result);
            printf("\n"); // Newline after result
            value_destroy(eval_result);
        }
        free(input_line);
    }
    return continue_repl;
}
