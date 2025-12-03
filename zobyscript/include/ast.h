#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

enum class ASTNodeType {
    NUMBER,
    STRING,
    BOOLEAN,
    NULLVAL,
    ARRAY,
    HASHMAP,
    INDEX,
    IDENTIFIER,
    BINARY_OP,
    UNARY_OP,
    ASSIGNMENT,
    FUNCTION_CALL,
    FUNCTION_DEF,
    RETURN,
    BLOCK,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    USE_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    TERNARY,
    PROGRAM
};

class ASTNode {
public:
    ASTNodeType type;
    virtual ~ASTNode() = default;
    
protected:
    ASTNode(ASTNodeType t) : type(t) {}
};

class NumberNode : public ASTNode {
public:
    double value;
    NumberNode(double v) : ASTNode(ASTNodeType::NUMBER), value(v) {}
};

class StringNode : public ASTNode {
public:
    std::string value;
    StringNode(const std::string& v) : ASTNode(ASTNodeType::STRING), value(v) {}
};

class BooleanNode : public ASTNode {
public:
    bool value;
    BooleanNode(bool v) : ASTNode(ASTNodeType::BOOLEAN), value(v) {}
};

class NullNode : public ASTNode {
public:
    NullNode() : ASTNode(ASTNodeType::NULLVAL) {}
};

class ArrayNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> elements;
    ArrayNode(std::vector<std::unique_ptr<ASTNode>> elems)
        : ASTNode(ASTNodeType::ARRAY), elements(std::move(elems)) {}
};

class HashMapNode : public ASTNode {
public:
    std::vector<std::pair<std::string, std::unique_ptr<ASTNode>>> pairs;
    HashMapNode(std::vector<std::pair<std::string, std::unique_ptr<ASTNode>>> p)
        : ASTNode(ASTNodeType::HASHMAP), pairs(std::move(p)) {}
};

class IndexNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> array;
    std::unique_ptr<ASTNode> index;
    IndexNode(std::unique_ptr<ASTNode> arr, std::unique_ptr<ASTNode> idx)
        : ASTNode(ASTNodeType::INDEX), array(std::move(arr)), index(std::move(idx)) {}
};

class IdentifierNode : public ASTNode {
public:
    std::string name;
    IdentifierNode(const std::string& n) : ASTNode(ASTNodeType::IDENTIFIER), name(n) {}
};

class BinaryOpNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    BinaryOpNode(const std::string& o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : ASTNode(ASTNodeType::BINARY_OP), op(o), left(std::move(l)), right(std::move(r)) {}
};

class UnaryOpNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> operand;
    
    UnaryOpNode(const std::string& o, std::unique_ptr<ASTNode> operand)
        : ASTNode(ASTNodeType::UNARY_OP), op(o), operand(std::move(operand)) {}
};

class AssignmentNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> value;
    
    AssignmentNode(const std::string& n, std::unique_ptr<ASTNode> v)
        : ASTNode(ASTNodeType::ASSIGNMENT), name(n), value(std::move(v)) {}
};

class FunctionCallNode : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    FunctionCallNode(const std::string& n, std::vector<std::unique_ptr<ASTNode>> args)
        : ASTNode(ASTNodeType::FUNCTION_CALL), name(n), arguments(std::move(args)) {}
};

class FunctionDefNode : public ASTNode {
public:
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<ASTNode> body;
    
    FunctionDefNode(const std::string& n, std::vector<std::string> p, std::unique_ptr<ASTNode> b)
        : ASTNode(ASTNodeType::FUNCTION_DEF), name(n), params(std::move(p)), body(std::move(b)) {}
};

class ReturnNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> value;
    
    ReturnNode(std::unique_ptr<ASTNode> v)
        : ASTNode(ASTNodeType::RETURN), value(std::move(v)) {}
};

class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    BlockNode(std::vector<std::unique_ptr<ASTNode>> stmts)
        : ASTNode(ASTNodeType::BLOCK), statements(std::move(stmts)) {}
};

class IfStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBranch;
    std::unique_ptr<ASTNode> elseBranch;
    
    IfStatementNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> then, std::unique_ptr<ASTNode> els = nullptr)
        : ASTNode(ASTNodeType::IF_STATEMENT), condition(std::move(cond)), thenBranch(std::move(then)), elseBranch(std::move(els)) {}
};

class WhileStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    
    WhileStatementNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> b)
        : ASTNode(ASTNodeType::WHILE_STATEMENT), condition(std::move(cond)), body(std::move(b)) {}
};

class ForStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> init;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> body;
    
    ForStatementNode(std::unique_ptr<ASTNode> i, std::unique_ptr<ASTNode> c, std::unique_ptr<ASTNode> inc, std::unique_ptr<ASTNode> b)
        : ASTNode(ASTNodeType::FOR_STATEMENT), init(std::move(i)), condition(std::move(c)), increment(std::move(inc)), body(std::move(b)) {}
};

class UseStatementNode : public ASTNode {
public:
    std::string library;
    UseStatementNode(const std::string& lib)
        : ASTNode(ASTNodeType::USE_STATEMENT), library(lib) {}
};

class BreakStatementNode : public ASTNode {
public:
    BreakStatementNode() : ASTNode(ASTNodeType::BREAK_STATEMENT) {}
};

class ContinueStatementNode : public ASTNode {
public:
    ContinueStatementNode() : ASTNode(ASTNodeType::CONTINUE_STATEMENT) {}
};

class TernaryNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenExpr;
    std::unique_ptr<ASTNode> elseExpr;
    TernaryNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> then, std::unique_ptr<ASTNode> els)
        : ASTNode(ASTNodeType::TERNARY), condition(std::move(cond)), thenExpr(std::move(then)), elseExpr(std::move(els)) {}
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    ProgramNode(std::vector<std::unique_ptr<ASTNode>> stmts)
        : ASTNode(ASTNodeType::PROGRAM), statements(std::move(stmts)) {}
};

#endif
