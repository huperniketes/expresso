/*
 * Expresso
 * evaluator.c
 *
 * The functions which interact with the parse tree to determine the value of
 * an expression. Functions here are driven by traversing the parse tree as
 * well as initiate the traversal.
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
#include "evaluator.h"
#include "parser_wrapper.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token types from Expresso.tokens
#define OP_ADD 15
#define OP_SUB 16
#define OP_MUL 17
#define OP_DIV 18
#define OP_MOD 19

// Forward declarations
Value visit_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree);
Value visit_additive_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree);
Value visit_multiplicative_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree);
Value visit_literal(CExpressoVisitor* visitor, ExpressoParseTree* tree);

Value evaluate_expression(ExpressoParseTree* tree) {
    if (tree == NULL) {
        return value_create_error("Cannot evaluate NULL parse tree.");
    }

    CExpressoVisitor visitor = {
        .user_data = NULL,
        .visit_expression = visit_expression,
        .visit_additive_expression = visit_additive_expression,
        .visit_multiplicative_expression = visit_multiplicative_expression,
        .visit_literal = visit_literal,
    };

    return expresso_tree_accept(tree, &visitor);
}

Value visit_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree) {
    ExpressoParseTree* child = expresso_tree_get_child(tree, 0);
    Value result = expresso_tree_accept(child, visitor);
    expresso_tree_destroy(child);
    return result;
}

Value visit_literal(CExpressoVisitor* visitor, ExpressoParseTree* tree) {
    const char* text = expresso_tree_get_text(tree);
    if (text[0] == '"') {
        size_t len = strlen(text);
        char* str = malloc(len - 1);
        strncpy(str, text + 1, len - 2);
        str[len - 2] = '\0';
        Value val = value_create_string(str);
        free(str);
        return val;
    } else {
        return value_create_integer(atoi(text));
    }
}

Value visit_additive_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree) {
    int child_count = expresso_tree_get_child_count(tree);
    if (child_count == 1) {
        return expresso_tree_accept(expresso_tree_get_child(tree, 0), visitor);
    }

    Value result = expresso_tree_accept(expresso_tree_get_child(tree, 0), visitor);

    for (int i = 1; i < child_count; i += 2) {
        int op_type = expresso_tree_get_terminal_type(expresso_tree_get_child(tree, i));
        Value right = expresso_tree_accept(expresso_tree_get_child(tree, i + 1), visitor);

        if (value_is_integer(result) && value_is_integer(right)) {
            int l = value_as_integer(result);
            int r = value_as_integer(right);
            value_destroy(result);
            if (op_type == OP_ADD) {
                result = value_create_integer(l + r);
            } else if (op_type == OP_SUB) {
                result = value_create_integer(l - r);
            }
        } else {
            value_destroy(result);
            result = value_create_error("Type error.");
        }
        value_destroy(right);
    }

    return result;
}

Value visit_multiplicative_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree) {
    int child_count = expresso_tree_get_child_count(tree);
    if (child_count == 1) {
        return expresso_tree_accept(expresso_tree_get_child(tree, 0), visitor);
    }

    Value result = expresso_tree_accept(expresso_tree_get_child(tree, 0), visitor);

    for (int i = 1; i < child_count; i += 2) {
        int op_type = expresso_tree_get_terminal_type(expresso_tree_get_child(tree, i));
        Value right = expresso_tree_accept(expresso_tree_get_child(tree, i + 1), visitor);

        if (value_is_integer(result) && value_is_integer(right)) {
            int l = value_as_integer(result);
            int r = value_as_integer(right);
            value_destroy(result);
            if (op_type == OP_MUL) {
                result = value_create_integer(l * r);
            } else if (op_type == OP_DIV) {
                if (r == 0) {
                    result = value_create_error("Division by zero.");
                } else {
                    result = value_create_integer(l / r);
                }
            } else if (op_type == OP_MOD) {
                if (r == 0) {
                    result = value_create_error("Division by zero.");
                } else {
                    result = value_create_integer(l % r);
                }
            }
        } else {
            value_destroy(result);
            result = value_create_error("Type error.");
        }
        value_destroy(right);
    }

    return result;
}
