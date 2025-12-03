#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include <map>
#include <string>

class Environment {
private:
    std::map<std::string, double> variables;

public:
    void set(const std::string& name, double value);
    double get(const std::string& name);
    bool exists(const std::string& name);
};

class Interpreter {
private:
    Environment env;
    
    double evaluate(ASTNode* node);
    void execute(ASTNode* node);
    void executeBuiltin(const std::string& name, const std::vector<double>& args);

public:
    void run(ProgramNode* program);
};

#endif
