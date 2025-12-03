#ifndef VM_H
#define VM_H

#include "bytecode.h"
#include "compiler.h"
#include <vector>
#include <map>
#include <string>

struct CallFrame {
    int returnIP;
    int basePointer;
    int functionId;
};

struct InlineCache {
    int globalIdx;
    Value cachedValue;
    bool valid;
    InlineCache() : globalIdx(-1), valid(false) {}
};

class VM {
private:
    std::vector<Value> stack;
    std::vector<CallFrame> callStack;
    std::vector<Value> globals;  // Changed from map to vector for O(1) access
    std::vector<InlineCache> globalCaches;
    const std::vector<Function>* functions;
    int ip;
    int bp;
    bool optimizationsEnabled;
    
    // Fast path registers for common operations
    Value fastReg[4];
    
    void push(const Value& value);
    Value pop();
    Value peek(int offset = 0);
    void executeChunk(const Chunk& chunk);
    bool isTruthy(const Value& value);
    Value callBuiltin(const std::string& name, const std::vector<Value>& args);

public:
    VM();
    void run(const Chunk& mainChunk, const std::vector<Function>& funcs);
    void enableOptimizations(bool enable) { optimizationsEnabled = enable; }
};

#endif
