/*
 * Expresso
 * value.c
 *
 * Functions for creating and manipulating the structures which represent
 * values in the Expresso runtime system.
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

static const char * value_type_names[] = {
	"Integer",
	"Float",
	"Character",
	"String",
	"Error"
};

// --- Value Creation Functions ---
Value value_create_integer(long long val) {
    Value v;
    v.type = VALUE_TYPE_INTEGER;
    v.data.integer_value = val;
    return v;
}

Value value_create_float(double val) {
    Value v;
    v.type = VALUE_TYPE_FLOAT;
    v.data.float_value = val;
    return v;
}

Value value_create_character(char val) {
    Value v;
    v.type = VALUE_TYPE_CHARACTER;
    v.data.char_value = val;
    return v;
}

Value value_create_string(const char* val) {
    Value v;
    v.type = VALUE_TYPE_STRING;
    if (val) {
        v.data.string_value = strdup(val); // Allocate and copy string
        if (!v.data.string_value) {
            // Handle allocation failure - this is a critical error
            fprintf(stderr, "Fatal Error: Memory allocation failed for string value.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        v.data.string_value = NULL;
    }
    return v;
}

Value value_create_error(const char* message) {
    Value v;
    v.type = VALUE_TYPE_ERROR;
    if (message) {
        v.data.string_value = strdup(message);
        if (!v.data.string_value) {
            fprintf(stderr, "Fatal Error: Memory allocation failed for error message.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        v.data.string_value = strdup("Unknown Error");
    }
    return v;
}

// --- Value Destruction Function ---
void value_destroy(Value val) {
    if (val.type == VALUE_TYPE_STRING || val.type == VALUE_TYPE_ERROR) {
        free(val.data.string_value);
    }
}

// --- Value Type Check Functions ---
bool value_is_integer(Value val) { return val.type == VALUE_TYPE_INTEGER; }
bool value_is_float(Value val) { return val.type == VALUE_TYPE_FLOAT; }
bool value_is_character(Value val) { return val.type == VALUE_TYPE_CHARACTER; }
bool value_is_string(Value val) { return val.type == VALUE_TYPE_STRING; }
bool value_is_error(Value val) { return val.type == VALUE_TYPE_ERROR; }

// --- Value Access Functions (with type checking) ---
// In a real system, these would have robust error handling or assertions
// For now, we assume correct type checking before calling these.
long long value_as_integer(Value val) {
    if (val.type == VALUE_TYPE_INTEGER) return val.data.integer_value;
    if (val.type == VALUE_TYPE_FLOAT) return (long long)val.data.float_value; // Implicit conversion
    if (val.type == VALUE_TYPE_CHARACTER) return (long long)val.data.char_value;
    // Error handling for incorrect type access would go here
    fprintf(stderr, "Error: Attempted to access non-integer value as integer.\n");
    exit(EXIT_FAILURE);
}

double value_as_float(Value val) {
    if (val.type == VALUE_TYPE_FLOAT) return val.data.float_value;
    if (val.type == VALUE_TYPE_INTEGER) return (double)val.data.integer_value;
    if (val.type == VALUE_TYPE_CHARACTER) return (double)val.data.char_value;
    fprintf(stderr, "Error: Attempted to access non-float value as float.\n");
    exit(EXIT_FAILURE);
}

char value_as_character(Value val) {
    if (val.type == VALUE_TYPE_CHARACTER) return val.data.char_value;
    if (val.type == VALUE_TYPE_INTEGER) return (char)val.data.integer_value; // Potential data loss
    fprintf(stderr, "Error: Attempted to access non-character value as character.\n");
    exit(EXIT_FAILURE);
}

const char* value_as_string(Value val) {
    if (val.type == VALUE_TYPE_STRING || val.type == VALUE_TYPE_ERROR) return val.data.string_value;
    fprintf(stderr, "Error: Attempted to access non-string value as string.\n");
    exit(EXIT_FAILURE);
}

const char* value_as_error_message(Value val) {
    if (val.type == VALUE_TYPE_ERROR) return val.data.string_value;
    fprintf(stderr, "Error: Attempted to access non-error value as error message.\n");
    exit(EXIT_FAILURE);
}

// --- Value Utility Functions ---
Value value_copy(Value val) {
    if (val.type == VALUE_TYPE_STRING || val.type == VALUE_TYPE_ERROR) {
        return value_create_string(val.data.string_value);
    }
    return val; // For other types, a shallow copy is fine (they are immutable primitives)
}

bool value_equals(Value v1, Value v2) {
    if (v1.type != v2.type) {
        // Implement type coercion for comparison if needed, e.g., 5 == 5.0
        // For now, strict type equality
        return false;
    }

    switch (v1.type) {
        case VALUE_TYPE_INTEGER: return v1.data.integer_value == v2.data.integer_value;
        case VALUE_TYPE_FLOAT:
            // Handle NaN comparison: NaN != NaN
            if (isnan(v1.data.float_value) || isnan(v2.data.float_value)) return false;
            return v1.data.float_value == v2.data.float_value;
        case VALUE_TYPE_CHARACTER: return v1.data.char_value == v2.data.char_value;
        case VALUE_TYPE_STRING: return strcmp(v1.data.string_value, v2.data.string_value) == 0;
        case VALUE_TYPE_ERROR: return strcmp(v1.data.string_value, v2.data.string_value) == 0;
    }
    return false; // Should not reach here
}

void value_print(Value val) {
    switch (val.type) {
        case VALUE_TYPE_INTEGER: printf("%lld", val.data.integer_value); break;
        case VALUE_TYPE_FLOAT: printf("%f", val.data.float_value); break;
        case VALUE_TYPE_CHARACTER: printf("'%c'", val.data.char_value); break;
        case VALUE_TYPE_STRING: printf("\"%s\"", val.data.string_value); break;
        case VALUE_TYPE_ERROR: fprintf(stderr, "Error: %s", val.data.string_value); break;
    }
}

// Added for error-reporting purposes
Value value_type_as_string(Value val) {
    switch (val.type) {
        case VALUE_TYPE_INTEGER:
        case VALUE_TYPE_FLOAT:
        case VALUE_TYPE_CHARACTER:
        case VALUE_TYPE_STRING:
        case VALUE_TYPE_ERROR:	return value_create_string(value_type_names[val.type]);
    }
    return value_create_error("Unknown value type.");
}

const char* value_c_str(Value val) {
    if (val.type == VALUE_TYPE_STRING || val.type == VALUE_TYPE_ERROR) return val.data.string_value;
    fprintf(stderr, "Error: Attempted to access non-string value as string.\n");
    exit(EXIT_FAILURE);
}
