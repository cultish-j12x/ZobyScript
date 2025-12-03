#include "../include/parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& toks) : tokens(toks), position(0) {}

Token& Parser::current() {
    if (position >= tokens.size()) {
        return tokens.back();
    }
    return tokens[position];
}

Token& Parser::peek() {
    if (position + 1 >= tokens.size()) {
        return tokens.back();
    }
    return tokens[position + 1];
}

bool Parser::match(TokenType type) {
    if (current().type == type) {
        position++;
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (current().type != type) {
        throw std::runtime_error(message);
    }
    position++;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    if (current().type == TokenType::NUMBER) {
        double value = std::stod(current().value);
        position++;
        return std::make_unique<NumberNode>(value);
    }
    else if (current().type == TokenType::STRING) {
        std::string value = current().value;
        position++;
        return std::make_unique<StringNode>(value);
    }
    else if (current().type == TokenType::TRUE) {
        position++;
        return std::make_unique<BooleanNode>(true);
    }
    else if (current().type == TokenType::FALSE) {
        position++;
        return std::make_unique<BooleanNode>(false);
    }
    else if (current().type == TokenType::NULLKW) {
        position++;
        return std::make_unique<NullNode>();
    }
    else if (current().type == TokenType::IDENTIFIER) {
        std::string name = current().value;
        position++;
        if (current().type == TokenType::LPAREN) {
            position--;
            return parseFunctionCall();
        }
        if (current().type == TokenType::LBRACKET) {
            position--;
            auto id = std::make_unique<IdentifierNode>(name);
            position++;
            consume(TokenType::LBRACKET, "Expected '['");
            auto index = parseLogicalOr();
            consume(TokenType::RBRACKET, "Expected ']'");
            return std::make_unique<IndexNode>(std::move(id), std::move(index));
        }
        return std::make_unique<IdentifierNode>(name);
    }
    else if (match(TokenType::LBRACKET)) {
        return parseArray();
    }
    else if (match(TokenType::LBRACE)) {
        std::vector<std::pair<std::string, std::unique_ptr<ASTNode>>> pairs;
        if (current().type != TokenType::RBRACE) {
            do {
                std::string key;
                if (current().type == TokenType::STRING) {
                    key = current().value;
                    position++;
                } else if (current().type == TokenType::IDENTIFIER) {
                    key = current().value;
                    position++;
                } else {
                    throw std::runtime_error("Expected string or identifier as hashmap key");
                }
                consume(TokenType::COLON, "Expected ':' after hashmap key");
                auto value = parseLogicalOr();
                pairs.push_back({key, std::move(value)});
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RBRACE, "Expected '}' after hashmap");
        return std::make_unique<HashMapNode>(std::move(pairs));
    }
    else if (match(TokenType::LPAREN)) {
        auto expr = parseLogicalOr();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw std::runtime_error("Expected number, string, boolean, identifier, or '('");
}

std::unique_ptr<ASTNode> Parser::parseUnary() {
    if (match(TokenType::MINUS)) {
        auto operand = parseUnary();
        return std::make_unique<UnaryOpNode>("-", std::move(operand));
    }
    if (match(TokenType::NOT)) {
        auto operand = parseUnary();
        return std::make_unique<UnaryOpNode>("!", std::move(operand));
    }
    return parseFactor();
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseUnary();
    
    while (current().type == TokenType::MULTIPLY || current().type == TokenType::DIVIDE) {
        std::string op = current().value;
        position++;
        auto right = parseUnary();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto left = parseTerm();
    
    while (current().type == TokenType::PLUS || current().type == TokenType::MINUS) {
        std::string op = current().value;
        position++;
        auto right = parseTerm();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseComparison() {
    auto left = parseExpression();
    
    while (current().type == TokenType::LESS || current().type == TokenType::GREATER ||
           current().type == TokenType::LESS_EQUAL || current().type == TokenType::GREATER_EQUAL ||
           current().type == TokenType::EQUAL || current().type == TokenType::NOT_EQUAL) {
        std::string op = current().value;
        position++;
        auto right = parseExpression();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto left = parseComparison();
    
    while (current().type == TokenType::AND) {
        position++;
        auto right = parseComparison();
        left = std::make_unique<BinaryOpNode>("and", std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseLogicalOr() {
    auto left = parseLogicalAnd();
    
    while (current().type == TokenType::OR) {
        position++;
        auto right = parseLogicalAnd();
        left = std::make_unique<BinaryOpNode>("or", std::move(left), std::move(right));
    }
    
    if (current().type == TokenType::QUESTION) {
        position++;
        auto thenExpr = parseLogicalOr();
        consume(TokenType::COLON, "Expected ':' in ternary operator");
        auto elseExpr = parseLogicalOr();
        return std::make_unique<TernaryNode>(std::move(left), std::move(thenExpr), std::move(elseExpr));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    std::string name = current().value;
    position++;
    consume(TokenType::ASSIGN, "Expected '=' in assignment");
    auto value = parseLogicalOr();
    return std::make_unique<AssignmentNode>(name, std::move(value));
}

std::unique_ptr<ASTNode> Parser::parseFunctionCall() {
    std::string name = current().value;
    position++;
    consume(TokenType::LPAREN, "Expected '(' after function name");
    
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    if (current().type != TokenType::RPAREN) {
        arguments.push_back(parseLogicalOr());
        
        while (match(TokenType::COMMA)) {
            arguments.push_back(parseLogicalOr());
        }
    }
    
    consume(TokenType::RPAREN, "Expected ')' after arguments");
    return std::make_unique<FunctionCallNode>(name, std::move(arguments));
}

std::unique_ptr<ASTNode> Parser::parseFunctionDef() {
    consume(TokenType::FUNC, "Expected 'func'");
    std::string name = current().value;
    consume(TokenType::IDENTIFIER, "Expected function name");
    consume(TokenType::LPAREN, "Expected '(' after function name");
    
    std::vector<std::string> params;
    if (current().type == TokenType::IDENTIFIER) {
        params.push_back(current().value);
        position++;
        while (match(TokenType::COMMA)) {
            params.push_back(current().value);
            consume(TokenType::IDENTIFIER, "Expected parameter name");
        }
    }
    
    consume(TokenType::RPAREN, "Expected ')' after parameters");
    auto body = parseBlock();
    return std::make_unique<FunctionDefNode>(name, std::move(params), std::move(body));
}

std::unique_ptr<ASTNode> Parser::parseReturn() {
    consume(TokenType::RETURN, "Expected 'return'");
    auto value = parseLogicalOr();
    return std::make_unique<ReturnNode>(std::move(value));
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    consume(TokenType::IF, "Expected 'if'");
    consume(TokenType::LPAREN, "Expected '(' after 'if'");
    auto condition = parseLogicalOr();
    consume(TokenType::RPAREN, "Expected ')' after condition");
    
    auto thenBranch = parseBlock();
    std::unique_ptr<ASTNode> elseBranch = nullptr;
    
    if (match(TokenType::ELSE)) {
        elseBranch = parseBlock();
    }
    
    return std::make_unique<IfStatementNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<ASTNode> Parser::parseWhileStatement() {
    consume(TokenType::WHILE, "Expected 'while'");
    consume(TokenType::LPAREN, "Expected '(' after 'while'");
    auto condition = parseLogicalOr();
    consume(TokenType::RPAREN, "Expected ')' after condition");
    
    auto body = parseBlock();
    return std::make_unique<WhileStatementNode>(std::move(condition), std::move(body));
}

std::unique_ptr<ASTNode> Parser::parseForStatement() {
    consume(TokenType::FOR, "Expected 'for'");
    consume(TokenType::LPAREN, "Expected '(' after 'for'");
    
    auto init = parseAssignment();
    if (match(TokenType::SEMICOLON)) {}
    
    auto condition = parseLogicalOr();
    if (match(TokenType::SEMICOLON)) {}
    
    auto increment = parseAssignment();
    consume(TokenType::RPAREN, "Expected ')' after for clauses");
    
    auto body = parseBlock();
    return std::make_unique<ForStatementNode>(std::move(init), std::move(condition), std::move(increment), std::move(body));
}

std::unique_ptr<ASTNode> Parser::parseUseStatement() {
    consume(TokenType::USE, "Expected 'use'");
    std::string libName = current().value;
    consume(TokenType::IDENTIFIER, "Expected library name");
    return std::make_unique<UseStatementNode>(libName);
}

std::unique_ptr<ASTNode> Parser::parseArray() {
    std::vector<std::unique_ptr<ASTNode>> elements;
    
    if (current().type != TokenType::RBRACKET) {
        elements.push_back(parseLogicalOr());
        while (match(TokenType::COMMA)) {
            elements.push_back(parseLogicalOr());
        }
    }
    
    consume(TokenType::RBRACKET, "Expected ']' after array elements");
    return std::make_unique<ArrayNode>(std::move(elements));
}

std::unique_ptr<ASTNode> Parser::parseBlock() {
    consume(TokenType::LBRACE, "Expected '{'");
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (current().type != TokenType::RBRACE && current().type != TokenType::END_OF_FILE) {
        statements.push_back(parseStatement());
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    return std::make_unique<BlockNode>(std::move(statements));
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (current().type == TokenType::USE) {
        return parseUseStatement();
    }
    if (current().type == TokenType::FUNC) {
        return parseFunctionDef();
    }
    if (current().type == TokenType::RETURN) {
        return parseReturn();
    }
    if (current().type == TokenType::BREAK) {
        position++;
        return std::make_unique<BreakStatementNode>();
    }
    if (current().type == TokenType::CONTINUE) {
        position++;
        return std::make_unique<ContinueStatementNode>();
    }
    if (current().type == TokenType::IF) {
        return parseIfStatement();
    }
    if (current().type == TokenType::WHILE) {
        return parseWhileStatement();
    }
    if (current().type == TokenType::FOR) {
        return parseForStatement();
    }
    if (current().type == TokenType::IDENTIFIER) {
        if (peek().type == TokenType::ASSIGN) {
            return parseAssignment();
        }
        else if (peek().type == TokenType::LPAREN) {
            return parseFunctionCall();
        }
    }
    
    throw std::runtime_error("Expected statement");
}

std::unique_ptr<ProgramNode> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (current().type != TokenType::END_OF_FILE) {
        statements.push_back(parseStatement());
    }
    
    return std::make_unique<ProgramNode>(std::move(statements));
}
