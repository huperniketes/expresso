#ifndef EXPRESSO_EVALUATOR_H
#define EXPRESSO_EVALUATOR_H

#include "value.h"
#include "parser_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

// Evaluate a parsed expression tree
Value evaluate_expression(ExpressoParseTree* tree);

#ifdef __cplusplus
}
#endif

#endif // EXPRESSO_EVALUATOR_H
