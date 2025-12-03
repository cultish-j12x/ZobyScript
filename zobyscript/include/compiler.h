#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"
#include "bytecode.h"
#include <map>
#include <set>
#include <string>
#include <memory>

struct Function {
    std::string name;
    int arity;
    Chunk chunk;
    std::vector<std::string> params;
};

class Compiler {
private:
    Chunk* currentChunk;
    std::map<std::string, int> globals;
    std::map<std::string, int> locals;
    std::map<std::string, int> functions;
    std::vector<Function> functionTable;
    std::set<std::string> loadedLibraries;
    std::map<std::string, Value> stringInternTable;
    int localCount;
    bool inFunction;
    bool obfuscate;
    bool optimizationsEnabled;
    std::string currentFunctionName;
    
    void compileExpression(ASTNode* node);
    void compileStatement(ASTNode* node);
    int resolveGlobal(const std::string& name);
    int resolveLocal(const std::string& name);
    void beginScope();
    void endScope();

    std::unique_ptr<ASTNode> optimizeConstantFolding(ASTNode* node);
    std::string* internString(const std::string& str);
    bool isTailCall(ASTNode* node, const std::string& funcName);
    void peepholeOptimize(Chunk& chunk);
    
public:
    Compiler();
    Chunk compile(ProgramNode* program);
    const std::vector<Function>& getFunctions() const { return functionTable; }
    void loadStandardLibrary(const std::string& libName);
    bool isObfuscated() const { return obfuscate; }
    void saveBytecode(const std::string& filename, const Chunk& chunk);
    Chunk loadBytecode(const std::string& filename);
    void enableOptimizations(bool enable) { optimizationsEnabled = enable; }
    void enablePeephole(bool enable) { optimizationsEnabled = enable; }
};

#endif
