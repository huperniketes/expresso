#ifndef EXPRESO_PARSER_WRAPPER_H
#define EXPRESO_PARSER_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration of the opaque parser context and parse tree
typedef struct ExpressoParserContext ExpressoParserContext;
typedef struct ExpressoParseTree ExpressoParseTree;

// Function to create a new parser instance
ExpressoParserContext* expresso_parser_create(void);

// Function to parse an expression string
// Returns the parse tree on success, NULL on syntax error
ExpressoParseTree* expresso_parser_parse(ExpressoParserContext* ctx, const char* expression_str);

// Get the raw text of a parse tree node
const char* expresso_tree_get_text(ExpressoParseTree* tree);

// Get the type of a parse tree node (returns enum value defined below)
int expresso_tree_get_type(ExpressoParseTree* tree);

// Free a parse tree
void expresso_tree_destroy(ExpressoParseTree* tree);

// Function to destroy the parser instance
void expresso_parser_destroy(ExpressoParserContext* ctx);

#ifdef __cplusplus
}
#endif

#endif // EXPRESO_PARSER_WRAPPER_H
