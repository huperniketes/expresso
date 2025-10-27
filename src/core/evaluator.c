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
    if (expresso_tree_get_child_count(tree) == 1) {
        ExpressoParseTree* child = expresso_tree_get_child(tree, 0);
        Value result = expresso_tree_accept(child, visitor);
        expresso_tree_destroy(child);
        return result;
    }

    ExpressoParseTree* left_child = expresso_tree_get_child(tree, 0);
    ExpressoParseTree* op_child = expresso_tree_get_child(tree, 1);
    int op_type = expresso_tree_get_terminal_type(op_child);
    ExpressoParseTree* right_child = expresso_tree_get_child(tree, 2);

    Value left_val = expresso_tree_accept(left_child, visitor);
    Value right_val = expresso_tree_accept(right_child, visitor);

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

Value visit_multiplicative_expression(CExpressoVisitor* visitor, ExpressoParseTree* tree) {
    if (expresso_tree_get_child_count(tree) == 1) {
        ExpressoParseTree* child = expresso_tree_get_child(tree, 0);
        Value result = expresso_tree_accept(child, visitor);
        expresso_tree_destroy(child);
        return result;
    }

    ExpressoParseTree* left_child = expresso_tree_get_child(tree, 0);
    ExpressoParseTree* op_child = expresso_tree_get_child(tree, 1);
    int op_type = expresso_tree_get_terminal_type(op_child);
    ExpressoParseTree* right_child = expresso_tree_get_child(tree, 2);

    Value left_val = expresso_tree_accept(left_child, visitor);
    Value right_val = expresso_tree_accept(right_child, visitor);

    if (value_is_error(left_val)) return left_val;
    if (value_is_error(right_val)) return right_val;

    if (!value_is_integer(left_val) || !value_is_integer(right_val)) {
        return value_create_error("Type error: operands must be integers.");
    }

    int left_int = value_as_integer(left_val);
    int right_int = value_as_integer(right_val);
    Value result;

    switch (op_type) {
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