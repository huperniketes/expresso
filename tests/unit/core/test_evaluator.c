#include "assert.h"
#include "evaluator.h"
#include "value.h"
#include "parser_wrapper.h" // Include parser_wrapper.h
#include <stdio.h>
#include <string.h>

void test_evaluate_1_plus_1() {
    ExpressoParserContext* parser_ctx = expresso_parser_create();
    ASSERT_TRUE(parser_ctx != NULL, "Failed to create parser context");

    ExpressoParseTree* tree = expresso_parser_parse(parser_ctx, "1+1");
    ASSERT_TRUE(tree != NULL, "Failed to parse '1+1'");

    Value result = evaluate_expression(tree);
    ASSERT_TRUE(value_is_integer(result), "Result of '1+1' should be integer");
    ASSERT_EQ(2, value_as_integer(result), "Result of '1+1' should be 2");
    value_destroy(result);
	expresso_tree_destroy(tree);
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
    test_evaluate_1_plus_1();
    test_evaluate_hello_string();
    test_evaluate_unknown_expression();
    printf("All Evaluator unit tests passed!\n");
    return 0;
}
