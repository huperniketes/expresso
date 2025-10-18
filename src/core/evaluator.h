#ifndef EXPRESO_EVALUATOR_H
#define EXPRESO_EVALUATOR_H

#include "value.h"

// Placeholder for the evaluation function
// This will now take a parse tree node
#ifdef __cplusplus
extern "C" {
#endif

Value evaluate_expression(void* parse_tree_ctx); // void* to avoid C++ headers in C file

#ifdef __cplusplus
}
#endif

#endif // EXPRESO_EVALUATOR_H
