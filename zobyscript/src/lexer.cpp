#include "../include/lexer.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& src) : source(src), position(0), line(1), column(1) {}

char Lexer::current() {
    if (position >= source.length()) return '\0';
    return source[position];
}

char Lexer::peek() {
    if (position + 1 >= source.length()) return '\0';
    return source[position + 1];
}

void Lexer::advance() {
    if (position < source.length()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void Lexer::skipWhitespace() {
    while (current() != '\0') {
        if (std::isspace(current())) {
            advance();
        } else if (current() == '#') {
            // Skip comment until end of line
            while (current() != '\0' && current() != '\n') {
                advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::number() {
    int startColumn = column;
    std::string num;
    
    while (current() != '\0' && (std::isdigit(current()) || current() == '.')) {
        num += current();
        advance();
    }
    
    return Token(TokenType::NUMBER, num, line, startColumn);
}

Token Lexer::identifier() {
    int startColumn = column;
    std::string id;
    
    while (current() != '\0' && (std::isalnum(current()) || current() == '_')) {
        id += current();
        advance();
    }
    
    if (id == "func") return Token(TokenType::FUNC, id, line, startColumn);
    if (id == "return") return Token(TokenType::RETURN, id, line, startColumn);
    if (id == "if") return Token(TokenType::IF, id, line, startColumn);
    if (id == "else") return Token(TokenType::ELSE, id, line, startColumn);
    if (id == "while") return Token(TokenType::WHILE, id, line, startColumn);
    if (id == "for") return Token(TokenType::FOR, id, line, startColumn);
    if (id == "true") return Token(TokenType::TRUE, id, line, startColumn);
    if (id == "false") return Token(TokenType::FALSE, id, line, startColumn);
    if (id == "and") return Token(TokenType::AND, id, line, startColumn);
    if (id == "or") return Token(TokenType::OR, id, line, startColumn);
    if (id == "not") return Token(TokenType::NOT, id, line, startColumn);
    if (id == "use") return Token(TokenType::USE, id, line, startColumn);
    if (id == "break") return Token(TokenType::BREAK, id, line, startColumn);
    if (id == "continue") return Token(TokenType::CONTINUE, id, line, startColumn);
    if (id == "try") return Token(TokenType::TRY, id, line, startColumn);
    if (id == "catch") return Token(TokenType::CATCH, id, line, startColumn);
    if (id == "throw") return Token(TokenType::THROW, id, line, startColumn);
    if (id == "null") return Token(TokenType::NULLKW, id, line, startColumn);
    
    return Token(TokenType::IDENTIFIER, id, line, startColumn);
}

Token Lexer::string() {
    int startColumn = column;
    advance(); // Skip opening quote
    std::string str;
    
    while (current() != '\0' && current() != '"') {
        if (current() == '\\' && peek() == '"') {
            advance();
            str += '"';
            advance();
        } else if (current() == '\\' && peek() == 'n') {
            advance();
            str += '\n';
            advance();
        } else {
            str += current();
            advance();
        }
    }
    
    if (current() == '"') advance(); // Skip closing quote
    return Token(TokenType::STRING, str, line, startColumn);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (current() != '\0') {
        skipWhitespace();
        
        if (current() == '\0') break;
        
        if (std::isdigit(current())) {
            tokens.push_back(number());
        }
        else if (std::isalpha(current()) || current() == '_') {
            tokens.push_back(identifier());
        }
        else if (current() == '+') {
            tokens.push_back(Token(TokenType::PLUS, "+", line, column));
            advance();
        }
        else if (current() == '-') {
            tokens.push_back(Token(TokenType::MINUS, "-", line, column));
            advance();
        }
        else if (current() == '=') {
            tokens.push_back(Token(TokenType::ASSIGN, "=", line, column));
            advance();
        }
        else if (current() == '(') {
            tokens.push_back(Token(TokenType::LPAREN, "(", line, column));
            advance();
        }
        else if (current() == ')') {
            tokens.push_back(Token(TokenType::RPAREN, ")", line, column));
            advance();
        }
        else if (current() == ',') {
            tokens.push_back(Token(TokenType::COMMA, ",", line, column));
            advance();
        }
        else if (current() == '*') {
            tokens.push_back(Token(TokenType::MULTIPLY, "*", line, column));
            advance();
        }
        else if (current() == '/') {
            tokens.push_back(Token(TokenType::DIVIDE, "/", line, column));
            advance();
        }
        else if (current() == '{') {
            tokens.push_back(Token(TokenType::LBRACE, "{", line, column));
            advance();
        }
        else if (current() == '}') {
            tokens.push_back(Token(TokenType::RBRACE, "}", line, column));
            advance();
        }
        else if (current() == '"') {
            tokens.push_back(string());
        }
        else if (current() == '<') {
            if (peek() == '=') {
                tokens.push_back(Token(TokenType::LESS_EQUAL, "<=", line, column));
                advance();
                advance();
            } else {
                tokens.push_back(Token(TokenType::LESS, "<", line, column));
                advance();
            }
        }
        else if (current() == '>') {
            if (peek() == '=') {
                tokens.push_back(Token(TokenType::GREATER_EQUAL, ">=", line, column));
                advance();
                advance();
            } else {
                tokens.push_back(Token(TokenType::GREATER, ">", line, column));
                advance();
            }
        }
        else if (current() == '=' && peek() == '=') {
            tokens.push_back(Token(TokenType::EQUAL, "==", line, column));
            advance();
            advance();
        }
        else if (current() == '!' && peek() == '=') {
            tokens.push_back(Token(TokenType::NOT_EQUAL, "!=", line, column));
            advance();
            advance();
        }
        else if (current() == '!') {
            tokens.push_back(Token(TokenType::NOT, "!", line, column));
            advance();
        }
        else if (current() == '[') {
            tokens.push_back(Token(TokenType::LBRACKET, "[", line, column));
            advance();
        }
        else if (current() == ']') {
            tokens.push_back(Token(TokenType::RBRACKET, "]", line, column));
            advance();
        }
        else if (current() == ';') {
            tokens.push_back(Token(TokenType::SEMICOLON, ";", line, column));
            advance();
        }
        else if (current() == '.') {
            tokens.push_back(Token(TokenType::DOT, ".", line, column));
            advance();
        }
        else if (current() == '?') {
            tokens.push_back(Token(TokenType::QUESTION, "?", line, column));
            advance();
        }
        else if (current() == ':') {
            tokens.push_back(Token(TokenType::COLON, ":", line, column));
            advance();
        }
        else {
            throw std::runtime_error("Unexpected character: " + std::string(1, current()));
        }
    }
    
    tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));
    return tokens;
}
