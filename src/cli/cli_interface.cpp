#include "cli_interface.h"
#include "parser_wrapper.h" // For C++ parser interface
#include "evaluator.h"      // For evaluator
#include "value.h"          // For Value type
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global parser context (managed by C++ wrapper)
static ExpressoParserContext* g_parser_ctx = NULL;

void cli_init() {
    g_parser_ctx = expresso_parser_create();
    if (!g_parser_ctx) {
        fprintf(stderr, "Fatal Error: Could not initialize parser context.\n");
        exit(EXIT_FAILURE);
    }
}

void cli_shutdown() {
    if (g_parser_ctx) {
        expresso_parser_destroy(g_parser_ctx);
        g_parser_ctx = NULL;
    }
}

Value cli_evaluate_expression(const char* input_line) {
    if (!g_parser_ctx) {
        return value_create_error("CLI interface not initialized.");
    }
    if (!input_line || strlen(input_line) == 0) {
        return value_create_error("Empty input provided for evaluation.");
    }

    ExpressoParser::ExpressionContext* tree = expresso_parser_parse(g_parser_ctx, input_line);
    if (tree != nullptr) { // No syntax errors
        Value eval_result = evaluate_expression(tree);
        // Note: The tree needs to be managed. For now, it's owned by the parser_ctx.
        // In a full implementation, the evaluator might take ownership or copy relevant parts.
        return eval_result;
    } else {
        // Error already printed by parser_wrapper
        return value_create_error("Syntax error during parsing.");
    }
}

