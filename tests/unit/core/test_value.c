#include "assert.h"
#include "value.h"
#include <string.h>
#include <stdio.h>

void test_value_create_integer() {
    Value v = value_create_integer(123);
    ASSERT_TRUE(value_is_integer(v), "Integer value type check failed");
    ASSERT_EQ(123, value_as_integer(v), "Integer value content mismatch");
    value_destroy(v); // No-op for integer
}

void test_value_create_float() {
    Value v = value_create_float(123.45);
    ASSERT_TRUE(value_is_float(v), "Float value type check failed");
    ASSERT_TRUE(value_as_float(v) > 123.44 && value_as_float(v) < 123.46, "Float value content mismatch");
    value_destroy(v); // No-op for float
}

void test_value_create_character() {
    Value v = value_create_character('A');
    ASSERT_TRUE(value_is_character(v), "Character value type check failed");
    ASSERT_EQ('A', value_as_character(v), "Character value content mismatch");
    value_destroy(v); // No-op for character
}

void test_value_create_string() {
    Value v = value_create_string("hello");
    ASSERT_TRUE(value_is_string(v), "String value type check failed");
    ASSERT_TRUE(strcmp("hello", value_c_str(v)) == 0, "String value content mismatch");
    value_destroy(v);
}

void test_value_create_error() {
    Value v = value_create_error("Test Error");
    ASSERT_TRUE(value_is_error(v), "Error value type check failed");
    ASSERT_TRUE(strcmp("Test Error", value_as_error_message(v)) == 0, "Error message content mismatch");
    value_destroy(v);
}

void test_value_copy_string() {
    Value original = value_create_string("original");
    Value copy = value_copy(original);
    ASSERT_TRUE(value_is_string(copy), "Copied value is not string");
    ASSERT_TRUE(strcmp(value_c_str(original), value_c_str(copy)) == 0, "Copied string content mismatch");
    ASSERT_TRUE(value_c_str(original) != value_c_str(copy), "Copied string is not a deep copy");
    value_destroy(original);
    value_destroy(copy);
}

void test_value_equals() {
    Value i1 = value_create_integer(10);
    Value i2 = value_create_integer(10);
    Value i3 = value_create_integer(20);
    Value f1 = value_create_float(10.0);
    Value s1 = value_create_string("test");
    Value s2 = value_create_string("test");
    Value s3 = value_create_string("other");

    ASSERT_TRUE(value_equals(i1, i2), "Integer equality failed");
    ASSERT_FALSE(value_equals(i1, i3), "Integer inequality failed");
    ASSERT_FALSE(value_equals(i1, f1), "Integer vs Float equality failed (strict)"); // Strict type equality
    ASSERT_TRUE(value_equals(s1, s2), "String equality failed");
    ASSERT_FALSE(value_equals(s1, s3), "String inequality failed");

    value_destroy(i1); value_destroy(i2); value_destroy(i3);
    value_destroy(f1);
    value_destroy(s1); value_destroy(s2); value_destroy(s3);
}


int main() {
    printf("Running Value type unit tests...\n");
    test_value_create_integer();
    test_value_create_float();
    test_value_create_character();
    test_value_create_string();
    test_value_create_error();
    test_value_copy_string();
    test_value_equals();
    printf("All Value type tests passed!\n");
    return 0;
}
