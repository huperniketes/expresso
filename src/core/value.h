/*
 * Expresso
 * value.h
 *
 * Header file with the declarations of the Value structure and functions
 * to represent values in the Expresso runtime system.
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
#ifndef EXPRESSO_VALUE_H
#define EXPRESSO_VALUE_H

#include <stdbool.h> // For bool
#include <stddef.h>  // For size_t

// Define the possible types a Value can hold
typedef enum {
    VALUE_TYPE_INTEGER,
    VALUE_TYPE_FLOAT,
    VALUE_TYPE_CHARACTER,
    VALUE_TYPE_STRING,
    VALUE_TYPE_ERROR // Special type for error propagation
} ValueType;

// Define the Value union/struct
typedef struct {
    ValueType type;
    union {
        long long integer_value; // Using long long for machine-dependent int
        double float_value;
        char char_value;
        char* string_value; // Dynamically allocated string
    } data;
} Value;

// --- Value Creation Functions ---
#ifdef __cplusplus
extern "C" {
#endif

Value value_create_integer(long long val);
Value value_create_float(double val);
Value value_create_character(char val);
Value value_create_string(const char* val);
Value value_create_error(const char* message); // For error propagation

// --- Value Destruction Function ---
void value_destroy(Value val);

// --- Value Type Check Functions ---
bool value_is_integer(Value val);
bool value_is_float(Value val);
bool value_is_character(Value val);
bool value_is_string(Value val);
bool value_is_error(Value val);

// --- Value Access Functions (with type checking) ---
long long value_as_integer(Value val);
double value_as_float(Value val);
char value_as_character(Value val);
const char* value_as_string(Value val); // Returns const char* for immutability
const char* value_as_error_message(Value val);

// --- Value Utility Functions ---
Value value_copy(Value val); // Creates a deep copy for strings
bool value_equals(Value v1, Value v2);
void value_print(Value val); // For debugging/output

#ifdef __cplusplus
}
#endif

#endif // EXPRESSO_VALUE_H
