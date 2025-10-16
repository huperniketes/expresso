#ifndef EXPRESO_CLI_INTERFACE_H
#define EXPRESO_CLI_INTERFACE_H

#include "value.h" // For Value type

// Initialize the CLI interface (e.g., parser context)
#ifdef __cplusplus
extern "C" {
#endif

void cli_init();

// Clean up the CLI interface
void cli_shutdown();

// Parse and evaluate an expression string
// Returns the result as a Value
Value cli_evaluate_expression(const char* input_line);

#ifdef __cplusplus
}
#endif

#endif // EXPRESO_CLI_INTERFACE_H
