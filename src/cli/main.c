/*
 * Expresso
 * main.c
 *
 * The high-level application lifecycle for the Expresso REPL.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "repl.h"
#include "evaluator.h"
#include "parser_wrapper.h"

int main(int argc, char* argv[]) {
    if (argc == 3 && strcmp(argv[1], "-e") == 0) {
        ExpressoParserContext* parser_ctx = expresso_parser_create();
        ExpressoParseTree* tree = expresso_parser_parse(parser_ctx, argv[2]);
        if (tree) {
            Value result = evaluate_expression(tree);
            value_print(result);
            printf("\n");
            value_destroy(result);
            expresso_tree_destroy(tree);
        }
        expresso_parser_destroy(parser_ctx);
        return 0;
    }

    const char *err_string = repl_init(); // Initialize CLI interface

    if (err_string) {
        fprintf(stderr, "Fatal Error: %s\n", err_string);
        repl_shutdown(); // Ensure cleanup
        return EXIT_FAILURE;
    }

    while(repl_read_eval_print() != 0)
        ;

    repl_shutdown(); // Clean up CLI interface
    return 0;
}
