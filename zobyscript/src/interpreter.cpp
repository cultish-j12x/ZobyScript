#include "../include/interpreter.h"
#include <iostream>
#include <stdexcept>

void Environment::set(const std::string& name, double value) {
    variables[name] = value;
}

double Environment::get(const std::string& name) {
    if (variables.find(name) == variables.end()) {
        throw std::runtime_error("Undefined variable: " + name);
    }
    return variables[name];
}

bool Environment::exists(const std::string& name) {
    return variables.find(name) != variables.end();
}

double Interpreter::evaluate(ASTNode* node) {
    if (node->type == ASTNodeType::NUMBER) {
        NumberNode* numNode = static_cast<NumberNode*>(node);
        return numNode->value;
    }
    else if (node->type == ASTNodeType::IDENTIFIER) {
        IdentifierNode* idNode = static_cast<IdentifierNode*>(node);
        return env.get(idNode->name);
    }
    else if (node->type == ASTNodeType::BINARY_OP) {
        BinaryOpNode* binNode = static_cast<BinaryOpNode*>(node);
        double left = evaluate(binNode->left.get());
        double right = evaluate(binNode->right.get());
        
        if (binNode->op == "+") {
            return left + right;
        }
        else if (binNode->op == "-") {
            return left - right;
        }
        
        throw std::runtime_error("Unknown operator: " + binNode->op);
    }
    
    throw std::runtime_error("Cannot evaluate node");
}

void Interpreter::executeBuiltin(const std::string& name, const std::vector<double>& args) {
    if (name == "print") {
        for (size_t i = 0; i < args.size(); i++) {
            std::cout << args[i];
            if (i < args.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;
    }
    else {
        throw std::runtime_error("Unknown function: " + name);
    }
}

void Interpreter::execute(ASTNode* node) {
    if (node->type == ASTNodeType::ASSIGNMENT) {
        AssignmentNode* assignNode = static_cast<AssignmentNode*>(node);
        double value = evaluate(assignNode->value.get());
        env.set(assignNode->name, value);
    }
    else if (node->type == ASTNodeType::FUNCTION_CALL) {
        FunctionCallNode* callNode = static_cast<FunctionCallNode*>(node);
        std::vector<double> args;
        
        for (auto& arg : callNode->arguments) {
            args.push_back(evaluate(arg.get()));
        }
        
        executeBuiltin(callNode->name, args);
    }
}

void Interpreter::run(ProgramNode* program) {
    for (auto& statement : program->statements) {
        execute(statement.get());
    }
}
