/*
 * Expresso
 * repl.c
 *
 * The functions which define the processes of the REPL's main loop:
 * read, eval, & print, and its initialization and cleanup.
 *
 * Expresso is an interactive command-line tool for evaluating expressions.
 * Expresso supports basic data types (integers, characters, strings),
 * literals, operators (arithmetic, logical, comparison, bitwise, string
 * manipulation), and nested expressions using C syntax (e.g., precedence,
 * parentheses). Key constraint: No mutable state—every input is a pure
 * expression that evaluates to an immutable value.
 *
 * ========================================================================
 *
 * © Copyright 2025 Alfonso Guerra
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of the copyright holder nor the names of
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "repl.h"
#include "parser_wrapper.h" // For C++ parser interface
#include "evaluator.h"      // For evaluator
#include "value.h"          // For Value type
#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static	History				*g_repl_history = NULL;

// Placeholder for a readline-like function
// This version will add to history, but not yet handle arrow keys
char* read_line(const char* prompt) {
    if (isatty(STDIN_FILENO)) {
        printf("%s", prompt);
        fflush(stdout);
    }
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

    return line;
}

// Global parser context (managed by C++ wrapper)
static ExpressoParserContext* g_parser_ctx = NULL;

const char* repl_init() {

    g_repl_history = history_create(10); // Create history with capacity 10 (FR-007)
    if (!g_repl_history) {
        return	"Could not create history buffer.";
    }

    g_parser_ctx = expresso_parser_create();
    if (!g_parser_ctx) {
        return	"Could not initialize parser context.";
    }

	return NULL;
}

void repl_shutdown() {

    if (g_parser_ctx) {
        expresso_parser_destroy(g_parser_ctx);
        g_parser_ctx = NULL;
    }

    if (g_repl_history) {
        history_destroy(g_repl_history);
        g_repl_history = NULL;
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

int read_eval_print(History *const h) {

    char* input_line = read_line("expr> ");
    int   continue_repl = input_line != NULL;

    if (continue_repl) {

        // Add to history
        if (strlen(input_line) > 0) {
            history_add(h, input_line);
        }
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
            printf("\n");
            value_destroy(eval_result);
        }
        free(input_line);
    }
    return continue_repl;
}

int repl_read_eval_print() {
	return	read_eval_print(g_repl_history);
}
