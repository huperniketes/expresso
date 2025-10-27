#include "assert.h"
#include "evaluator.h"
#include "value.h"
#include "parser_wrapper.h" // Include parser_wrapper.h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void test_evaluate_arithmetic_operations() {
    ExpressoParserContext* parser_ctx = expresso_parser_create();
    ASSERT_TRUE(parser_ctx != NULL, "Failed to create parser context");
    srand(time(NULL));

    for (int i = 0; i < 10; i++) {
        int a = rand() % 100;
        int b = rand() % 99 + 1; // Avoid division by zero

        char expr_str[32];
        char assert_msg[128];
        ExpressoParseTree* tree;
        Value result;

        // Test addition
        sprintf(expr_str, "%d + %d", a, b);
        tree = expresso_parser_parse(parser_ctx, expr_str);
        ASSERT_TRUE(tree != NULL, "Failed to parse addition");
        result = evaluate_expression(tree);
        ASSERT_TRUE(value_is_integer(result), "Addition result should be integer");
        snprintf(assert_msg, sizeof(assert_msg), "Addition failed: %d + %d != %lld", a, b, value_as_integer(result));
        ASSERT_EQ(a + b, value_as_integer(result), assert_msg);
        value_destroy(result);
        expresso_tree_destroy(tree);

        // Test subtraction
        sprintf(expr_str, "%d - %d", a, b);
        tree = expresso_parser_parse(parser_ctx, expr_str);
        ASSERT_TRUE(tree != NULL, "Failed to parse subtraction");
        result = evaluate_expression(tree);
        ASSERT_TRUE(value_is_integer(result), "Subtraction result should be integer");
        snprintf(assert_msg, sizeof(assert_msg), "Subtraction failed: %d - %d != %lld", a, b, value_as_integer(result));
        ASSERT_EQ(a - b, value_as_integer(result), assert_msg);
        value_destroy(result);
        expresso_tree_destroy(tree);

        // Test multiplication
        sprintf(expr_str, "%d * %d", a, b);
        tree = expresso_parser_parse(parser_ctx, expr_str);
        ASSERT_TRUE(tree != NULL, "Failed to parse multiplication");
        result = evaluate_expression(tree);
        ASSERT_TRUE(value_is_integer(result), "Multiplication result should be integer");
        snprintf(assert_msg, sizeof(assert_msg), "Multiplication failed: %d * %d != %lld", a, b, value_as_integer(result));
        ASSERT_EQ(a * b, value_as_integer(result), assert_msg);
        value_destroy(result);
        expresso_tree_destroy(tree);

        // Test division
        sprintf(expr_str, "%d / %d", a, b);
        tree = expresso_parser_parse(parser_ctx, expr_str);
        ASSERT_TRUE(tree != NULL, "Failed to parse division");
        result = evaluate_expression(tree);
        ASSERT_TRUE(value_is_integer(result), "Division result should be integer");
        snprintf(assert_msg, sizeof(assert_msg), "Division failed: %d / %d != %lld", a, b, value_as_integer(result));
        ASSERT_EQ(a / b, value_as_integer(result), assert_msg);
        value_destroy(result);
        expresso_tree_destroy(tree);

        // Test modulo
        sprintf(expr_str, "%d %% %d", a, b);
        tree = expresso_parser_parse(parser_ctx, expr_str);
        ASSERT_TRUE(tree != NULL, "Failed to parse modulo");
        result = evaluate_expression(tree);
        ASSERT_TRUE(value_is_integer(result), "Modulo result should be integer");
        snprintf(assert_msg, sizeof(assert_msg), "Modulo failed: %d %% %d != %lld", a, b, value_as_integer(result));
        ASSERT_EQ(a % b, value_as_integer(result), assert_msg);
        value_destroy(result);
        expresso_tree_destroy(tree);
    }

    expresso_parser_destroy(parser_ctx);
}

void test_evaluate_hello_string() {
    ExpressoParserContext* parser_ctx = expresso_parser_create();
    ASSERT_TRUE(parser_ctx != NULL, "Failed to create parser context");

    ExpressoParseTree* tree = expresso_parser_parse(parser_ctx, "\"hello\"");
    ASSERT_TRUE(tree != NULL, "Failed to parse '\"hello\"'");

    Value result = evaluate_expression(tree);
    ASSERT_TRUE(value_is_string(result), "Result of '\"hello\"' should be string");
    ASSERT_TRUE(strcmp("hello", value_as_string(result)) == 0, "Result of '\"hello\"' should be \"hello\"");
    value_destroy(result);
    expresso_tree_destroy(tree);
    expresso_parser_destroy(parser_ctx);
}

void test_evaluate_unknown_expression() {
    ExpressoParserContext* parser_ctx = expresso_parser_create();
    ASSERT_TRUE(parser_ctx != NULL, "Failed to create parser context");

    ExpressoParseTree* tree = expresso_parser_parse(parser_ctx, "x");
    ASSERT_TRUE(tree == NULL, "Incorrectly parsed 'x'");

    Value result = evaluate_expression(tree);
    ASSERT_TRUE(value_is_error(result), "Result of unknown expression should be error");
    ASSERT_TRUE(strstr(value_as_error_message(result), "Cannot evaluate") != NULL, "Error message should contain 'Cannot evaluate'");
    value_destroy(result);
    expresso_tree_destroy(tree);
    expresso_parser_destroy(parser_ctx);
}

int main() {
    printf("Running Evaluator unit tests...\n");
    test_evaluate_arithmetic_operations();
    test_evaluate_hello_string();
    test_evaluate_unknown_expression();
    printf("All Evaluator unit tests passed!\n");
    return 0;
}
