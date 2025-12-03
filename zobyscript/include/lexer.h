#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

enum class TokenType {
    NUMBER,
    STRING,
    IDENTIFIER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COMMA,
    SEMICOLON,
    DOT,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    EQUAL,
    NOT_EQUAL,
    AND,
    OR,
    NOT,
    FUNC,
    RETURN,
    IF,
    ELSE,
    WHILE,
    FOR,
    TRUE,
    FALSE,
    USE,
    BREAK,
    CONTINUE,
    TRY,
    CATCH,
    THROW,
    NULLKW,
    QUESTION,
    COLON,
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;

    char current();
    char peek();
    void advance();
    void skipWhitespace();
    Token number();
    Token identifier();
    Token string();

public:
    Lexer(const std::string& src);
    std::vector<Token> tokenize();
};

#endif
