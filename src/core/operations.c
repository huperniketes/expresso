/*
 * Expresso
 * operations.c
 *
 * Implementation of operations for the Expresso expression evaluator 
 * that work with the Value type system. Provides functions for
 * mathematical (arithmetical, relational, bitwise, logical)
 * operations with type checking and error handling.
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
#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // For isnan, isinf

// --- Value Operations Functions ---
Value value_by_adding_values(Value leftValue, Value rightValue) {
    Value v;

    if (value_is_integer(leftValue) && value_is_integer(rightValue))
    {
        v.type = VALUE_TYPE_INTEGER;
        int l = value_as_integer(leftValue);
        int r = value_as_integer(rightValue);
        v.data.integer_value = l + r;
    }
    else
    {
        v = value_create_error("Type error.");
    }
    return v;
}

Value value_by_subtracting_values(Value leftValue, Value rightValue) {
    Value v;

    if (value_is_integer(leftValue) && value_is_integer(rightValue))
    {
        v.type = VALUE_TYPE_INTEGER;
        int l = value_as_integer(leftValue);
        int r = value_as_integer(rightValue);
        v.data.integer_value = l - r;
    }
    else
    {
        v = value_create_error("Type error.");
    }
    return v;
}

Value value_by_multiplying_values(Value leftValue, Value rightValue) {
    Value v;

    if (value_is_integer(leftValue) && value_is_integer(rightValue))
    {
        v.type = VALUE_TYPE_INTEGER;
        int l = value_as_integer(leftValue);
        int r = value_as_integer(rightValue);
        v.data.integer_value = l * r;
    }
    else
    {
        v = value_create_error("Type error.");
    }
    return v;
}

Value value_by_dividing_values(Value leftValue, Value rightValue) {
    Value v;

    if (value_is_integer(leftValue) && value_is_integer(rightValue))
    {
        v.type = VALUE_TYPE_INTEGER;
        int l = value_as_integer(leftValue);
        int r = value_as_integer(rightValue);
        v.data.integer_value = l / r;
    }
    else
    {
        v = value_create_error("Type error.");
    }
    return v;
}

Value value_by_modulasing_values(Value leftValue, Value rightValue) {
    Value v;

    if (value_is_integer(leftValue) && value_is_integer(rightValue))
    {
        v.type = VALUE_TYPE_INTEGER;
        int l = value_as_integer(leftValue);
        int r = value_as_integer(rightValue);
        v.data.integer_value = l % r;
    }
    else
    {
        v = value_create_error("Type error.");
    }
    return v;
}

Value value_by_negating_value(Value value) {
    Value v;
    if (value_is_integer(value)) {
        v.type = VALUE_TYPE_INTEGER;
        v.data.integer_value = -value_as_integer(value);
    } else {
        v = value_create_error("Type error for negation.");
    }
    return v;
}

Value value_by_logical_negating_value(Value value) {
    Value v;
    if (value_is_integer(value)) {
        v.type = VALUE_TYPE_INTEGER;
        v.data.integer_value = !value_as_integer(value);
    } else {
        v = value_create_error("Type error for logical NOT.");
    }
    return v;
}

Value value_by_bitwise_complementing_value(Value value) {
    Value v;
    if (value_is_integer(value)) {
        v.type = VALUE_TYPE_INTEGER;
        v.data.integer_value = ~value_as_integer(value);
    } else {
        v = value_create_error("Type error for bitwise NOT.");
    }
    return v;
}
