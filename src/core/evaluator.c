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
#include "operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token types from Expresso.tokens
#define OP_ADD 15
#define OP_SUB 16
#define OP_MUL 17
#define OP_DIV 18
#define OP_MOD 19
#define OP_NOT 20
#define OP_BIT_NOT 21

// Forward declarations
Value visit_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree);
Value visit_additive_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree);
Value visit_multiplicative_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree);
Value visit_unary_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree);
Value visit_primary_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree);
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
        .visit_unary_expression = visit_unary_expression,
        .visit_primary_expression = visit_primary_expression,
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
        Value nextResult;

        if (op_type == OP_ADD) {
            nextResult = value_by_adding_values(result, right);
            value_destroy(result);
            result = nextResult;
        } else if (op_type == OP_SUB) {
            nextResult = value_by_subtracting_values(result, right);
            value_destroy(result);
            result = nextResult;
        } else {
            i = child_count;
            value_destroy(result);
            result = value_create_error("Unknown operator.");
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
        Value nextResult;

        if (op_type == OP_MUL) {
            nextResult = value_by_multiplying_values(result, right);
            value_destroy(result);
            result = nextResult;
        } else if (op_type == OP_DIV) {
            nextResult = value_by_dividing_values(result, right);
            value_destroy(result);
            result = nextResult;
        } else if (op_type == OP_MOD) {
            nextResult = value_by_modulasing_values(result, right);
            value_destroy(result);
            result = nextResult;
        } else {
            i = child_count;
            value_destroy(result);
            result = value_create_error("Unknown operator.");
        }
        value_destroy(right);
    }

    return result;
}

Value visit_unary_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree) {
    int child_count = expresso_tree_get_child_count(tree);
    if (child_count == 1) {
        return expresso_tree_accept(expresso_tree_get_child(tree, 0), visitor);
    }

    int op_type = expresso_tree_get_terminal_type(expresso_tree_get_child(tree, 0));
    Value operand = expresso_tree_accept(expresso_tree_get_child(tree, 1), visitor);
    Value result;

    switch (op_type) {
        case OP_ADD:
            // Unary plus, no-op
            result = operand;
            break;
        case OP_SUB:
            result = value_by_negating_value(operand);
            value_destroy(operand);
            break;
        case OP_NOT:
            result = value_by_logical_negating_value(operand);
            value_destroy(operand);
            break;
        case OP_BIT_NOT:
            result = value_by_bitwise_complementing_value(operand);
            value_destroy(operand);
            break;
        default:
            value_destroy(operand);
            result = value_create_error("Unknown unary operator.");
            break;
    }

    return result;
}

Value visit_primary_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree) {
    ExpressoParseTree* child = expresso_tree_get_child(tree, 0);
    int child_type = expresso_tree_get_type(child);

    // If the child is a literal, visit it.
    if (child_type == RuleLiteral) {
        return expresso_tree_accept(child, visitor);
    } else if (expresso_tree_get_child_count(tree) == 3) {
        // If it's a parenthesized expression, visit the expression inside.
        ExpressoParseTree* inner_expression = expresso_tree_get_child(tree, 1);
        Value result = expresso_tree_accept(inner_expression, visitor);
        expresso_tree_destroy(inner_expression);
        return result;
    }

    return value_create_error("Invalid primary expression");
}
