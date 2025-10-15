#ifndef EXPRESO_PARSER_WRAPPER_H
#define EXPRESO_PARSER_WRAPPER_H

#include "ExpressoParser.h" // Include for ExpressoParser::ExpressionContext

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration of the opaque parser context
typedef struct ExpressoParserContext ExpressoParserContext;

// Function to create a new parser instance
ExpressoParserContext* expresso_parser_create();

// Function to parse an expression string
// Returns the parse tree (ExpressoParser::ExpressionContext*) on success, NULL on syntax error.
ExpressoParser::ExpressionContext* expresso_parser_parse(ExpressoParserContext* ctx, const char* expression_str);

// Function to destroy the parser instance
void expresso_parser_destroy(ExpressoParserContext* ctx);

#ifdef __cplusplus
}
#endif

#endif // EXPRESO_PARSER_WRAPPER_H
