#include "evaluator.h"
#include "parser_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Rule indices from ExpressoParser.h
#define RULE_additiveExpression 10
#define RULE_multiplicativeExpression 11
#define RULE_literal 14

// Token types from Expresso.tokens
#define OP_ADD 15
#define OP_SUB 16
#define OP_MUL 17
#define OP_DIV 18
#define OP_MOD 19

Value evaluate_expression(ExpressoParseTree* tree) {
    if (tree == NULL) {
        return value_create_error("Cannot evaluate NULL parse tree.");
    }

    int type = expresso_tree_get_type(tree);

    switch (type) {
        case RULE_literal: {
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

        case RULE_additiveExpression:
        case RULE_multiplicativeExpression: {
            if (expresso_tree_get_child_count(tree) == 1) {
                ExpressoParseTree* child = expresso_tree_get_child(tree, 0);
                Value result = evaluate_expression(child);
                expresso_tree_destroy(child);
                return result;
            }

            ExpressoParseTree* left_child = expresso_tree_get_child(tree, 0);
            ExpressoParseTree* op_child = expresso_tree_get_child(tree, 1);
            int op_type = expresso_tree_get_terminal_type(op_child);
            ExpressoParseTree* right_child = expresso_tree_get_child(tree, 2);

            Value left_val = evaluate_expression(left_child);
            Value right_val = evaluate_expression(right_child);

            if (value_is_error(left_val)) return left_val;
            if (value_is_error(right_val)) return right_val;

            if (!value_is_integer(left_val) || !value_is_integer(right_val)) {
                return value_create_error("Type error: operands must be integers.");
            }

            int left_int = value_as_integer(left_val);
            int right_int = value_as_integer(right_val);
            Value result;

            switch (op_type) {
                case OP_ADD:
                    result = value_create_integer(left_int + right_int);
                    break;
                case OP_SUB:
                    result = value_create_integer(left_int - right_int);
                    break;
                case OP_MUL:
                    result = value_create_integer(left_int * right_int);
                    break;
                case OP_DIV:
                    if (right_int == 0) {
                        result = value_create_error("Division by zero.");
                    } else {
                        result = value_create_integer(left_int / right_int);
                    }
                    break;
                case OP_MOD:
                    if (right_int == 0) {
                        result = value_create_error("Division by zero.");
                    } else {
                        result = value_create_integer(left_int % right_int);
                    }
                    break;
                default:
                    result = value_create_error("Unknown operator.");
            }

            value_destroy(left_val);
            value_destroy(right_val);
            expresso_tree_destroy(left_child);
            expresso_tree_destroy(op_child);
            expresso_tree_destroy(right_child);

            return result;
        }

        default: {
            if (expresso_tree_get_child_count(tree) > 0) {
                ExpressoParseTree* child = expresso_tree_get_child(tree, 0);
                Value result = evaluate_expression(child);
                expresso_tree_destroy(child);
                return result;
            }
        }
    }

    return value_create_error("Cannot evaluate expression");
}
