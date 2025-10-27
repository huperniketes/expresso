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
