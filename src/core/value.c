#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // For isnan, isinf

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
