#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <vector>
#include <memory>

class Parser {
private:
    std::vector<Token> tokens;
    size_t position;

    Token& current();
    Token& peek();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseFunctionCall();
    std::unique_ptr<ASTNode> parseFunctionDef();
    std::unique_ptr<ASTNode> parseReturn();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseWhileStatement();
    std::unique_ptr<ASTNode> parseForStatement();
    std::unique_ptr<ASTNode> parseUseStatement();
    std::unique_ptr<ASTNode> parseArray();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();

public:
    Parser(const std::vector<Token>& toks);
    std::unique_ptr<ProgramNode> parse();
};

#endif
