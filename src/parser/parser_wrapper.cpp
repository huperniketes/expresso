#include "parser_wrapper.h"
#include "ExpressoLexer.h"
#include "ExpressoParser.h"
#include "ExpressoBaseVisitor.h"
#include "antlr4-runtime.h"
#include <iostream>
#include <string>

// Define the opaque context structure
struct ExpressoParserContext {
    antlr4::ANTLRInputStream input;
    ExpressoLexer lexer;
    antlr4::CommonTokenStream tokens;
    ExpressoParser parser;

    ExpressoParserContext() :
        input(""), lexer(&input), tokens(&lexer), parser(&tokens) {}
};

// Define the parse tree wrapper structure
struct ExpressoParseTree {
    antlr4::tree::ParseTree* node;
    std::string text;

    explicit ExpressoParseTree(antlr4::tree::ParseTree* n) : node(n) {
        if (node) {
            text = node->getText();
        }
    }
};

ExpressoParserContext* expresso_parser_create(void) {
    return new ExpressoParserContext();
}

ExpressoParseTree* expresso_parser_parse(ExpressoParserContext* ctx, const char* expression_str) {
    if (!ctx || !expression_str) return nullptr;

    ctx->input.load(expression_str);
    ctx->lexer.setInputStream(&ctx->input);
    ctx->tokens.setTokenSource(&ctx->lexer);
    ctx->parser.setTokenStream(&ctx->tokens);
    ctx->parser.reset();

    ExpressoParser::ExpressionContext* tree = ctx->parser.expression();

    if (ctx->parser.getNumberOfSyntaxErrors() > 0) {
        std::cerr << "Syntax Error(s) detected." << std::endl;
        return nullptr;
    }

    return new ExpressoParseTree(tree);
}

const char* expresso_tree_get_text(ExpressoParseTree* tree) {
    if (!tree) return nullptr;
    return tree->text.c_str();
}

int expresso_tree_get_type(ExpressoParseTree* tree) {
    if (!tree || !tree->node) return -1;
    if (auto* rule_node = dynamic_cast<antlr4::RuleContext*>(tree->node)) {
        return rule_node->getRuleIndex();
    }
    return -1; // Terminal node
}

int expresso_tree_get_child_count(ExpressoParseTree* tree) {
    if (!tree || !tree->node) return 0;
    return tree->node->children.size();
}

ExpressoParseTree* expresso_tree_get_child(ExpressoParseTree* tree, int index) {
    if (!tree || !tree->node) return nullptr;
    if (index < 0 || (size_t)index >= tree->node->children.size()) {
        return nullptr;
    }
    return new ExpressoParseTree(tree->node->children[index]);
}

int expresso_tree_get_terminal_type(ExpressoParseTree* tree) {
    if (!tree || !tree->node) return -1;
    if (auto* term_node = dynamic_cast<antlr4::tree::TerminalNode*>(tree->node)) {
        return term_node->getSymbol()->getType();
    }
    return -1;
}

void expresso_tree_destroy(ExpressoParseTree* tree) {
    delete tree;
}

// Visitor implementation
class CxxVisitor : public ExpressoBaseVisitor {
public:
    CxxVisitor(CExpressoVisitor* visitor) : visitor_(visitor) {}

    std::any visitExpression(ExpressoParser::ExpressionContext *ctx) override {
        if (visitor_->visit_expression) {
            ExpressoParseTree tree(ctx);
            return visitor_->visit_expression(visitor_, &tree);
        }
        return visitChildren(ctx);
    }

    std::any visitAdditiveExpression(ExpressoParser::AdditiveExpressionContext *ctx) override {
        if (visitor_->visit_additive_expression) {
            ExpressoParseTree tree(ctx);
            return visitor_->visit_additive_expression(visitor_, &tree);
        }
        return visitChildren(ctx);
    }

    std::any visitMultiplicativeExpression(ExpressoParser::MultiplicativeExpressionContext *ctx) override {
        if (visitor_->visit_multiplicative_expression) {
            ExpressoParseTree tree(ctx);
            return visitor_->visit_multiplicative_expression(visitor_, &tree);
        }
        return visitChildren(ctx);
    }

    std::any visitLiteral(ExpressoParser::LiteralContext *ctx) override {
        if (visitor_->visit_literal) {
            ExpressoParseTree tree(ctx);
            return visitor_->visit_literal(visitor_, &tree);
        }
        return visitChildren(ctx);
    }

private:
    CExpressoVisitor* visitor_;
};

Value expresso_tree_accept(ExpressoParseTree* tree, CExpressoVisitor* visitor) {
    if (!tree || !tree->node || !visitor) {
        return value_create_error("Invalid arguments to accept");
    }
    CxxVisitor c_visitor(visitor);
    std::any result = c_visitor.visit(tree->node);
    if (result.has_value() && result.type() == typeid(Value)) {
        return std::any_cast<Value>(result);
    }
    return value_create_error("Visitor did not return a Value");
}

void expresso_parser_destroy(ExpressoParserContext* ctx) {
    delete ctx;
}
