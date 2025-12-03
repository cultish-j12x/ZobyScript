#include "../include/compiler.h"
#include <stdexcept>
#include <fstream>

Compiler::Compiler() : currentChunk(nullptr), localCount(0), inFunction(false), obfuscate(false), optimizationsEnabled(true), currentFunctionName("") {}

std::string* Compiler::internString(const std::string& str) {
    if (stringInternTable.find(str) == stringInternTable.end()) {
        stringInternTable[str] = Value(str);
    }
    return &stringInternTable[str].string;
}

std::unique_ptr<ASTNode> Compiler::optimizeConstantFolding(ASTNode* node) {
    if (!optimizationsEnabled || node->type != ASTNodeType::BINARY_OP) {
        return nullptr;
    }
    
    BinaryOpNode* binNode = static_cast<BinaryOpNode*>(node);
    
    // Check if both operands are numbers
    if (binNode->left->type == ASTNodeType::NUMBER && binNode->right->type == ASTNodeType::NUMBER) {
        NumberNode* left = static_cast<NumberNode*>(binNode->left.get());
        NumberNode* right = static_cast<NumberNode*>(binNode->right.get());
        
        double result = 0;
        if (binNode->op == "+") result = left->value + right->value;
        else if (binNode->op == "-") result = left->value - right->value;
        else if (binNode->op == "*") result = left->value * right->value;
        else if (binNode->op == "/") result = left->value / right->value;
        else return nullptr;
        
        return std::make_unique<NumberNode>(result);
    }
    
    return nullptr;
}

void Compiler::loadStandardLibrary(const std::string& libName) {
    if (loadedLibraries.find(libName) != loadedLibraries.end()) return;
    loadedLibraries.insert(libName);
    
    if (libName == "obfuscator") {
        obfuscate = true;
    }
}

int Compiler::resolveGlobal(const std::string& name) {
    if (globals.find(name) == globals.end()) {
        globals[name] = static_cast<int>(globals.size());
    }
    return globals[name];
}

int Compiler::resolveLocal(const std::string& name) {
    if (locals.find(name) != locals.end()) {
        return locals[name];
    }
    return -1;
}

void Compiler::beginScope() {
    localCount = 0;
    locals.clear();
}

void Compiler::endScope() {
    locals.clear();
    localCount = 0;
}

void Compiler::compileExpression(ASTNode* node) {
    if (node->type == ASTNodeType::NUMBER) {
        NumberNode* numNode = static_cast<NumberNode*>(node);
        if (optimizationsEnabled && numNode->value == 0.0) {
            currentChunk->write(OpCode::OP_CONSTANT_0);
        } else if (optimizationsEnabled && numNode->value == 1.0) {
            currentChunk->write(OpCode::OP_CONSTANT_1);
        } else {
            int constIdx = currentChunk->addConstant(Value(numNode->value));
            currentChunk->write(OpCode::OP_CONSTANT);
            currentChunk->write(constIdx);
        }
    }
    else if (node->type == ASTNodeType::STRING) {
        StringNode* strNode = static_cast<StringNode*>(node);
        int constIdx = currentChunk->addConstant(Value(strNode->value));
        currentChunk->write(OpCode::OP_STRING);
        currentChunk->write(constIdx);
    }
    else if (node->type == ASTNodeType::BOOLEAN) {
        BooleanNode* boolNode = static_cast<BooleanNode*>(node);
        currentChunk->write(boolNode->value ? OpCode::OP_TRUE : OpCode::OP_FALSE);
    }
    else if (node->type == ASTNodeType::NULLVAL) {
        currentChunk->write(OpCode::OP_NULL);
    }
    else if (node->type == ASTNodeType::HASHMAP) {
        HashMapNode* hmNode = static_cast<HashMapNode*>(node);
        for (auto& pair : hmNode->pairs) {
            int keyIdx = currentChunk->addConstant(Value(pair.first));
            currentChunk->write(OpCode::OP_STRING);
            currentChunk->write(keyIdx);
            compileExpression(pair.second.get());
        }
        currentChunk->write(OpCode::OP_HASHMAP);
        currentChunk->write(static_cast<uint8_t>(hmNode->pairs.size()));
    }
    else if (node->type == ASTNodeType::TERNARY) {
        TernaryNode* ternNode = static_cast<TernaryNode*>(node);
        compileExpression(ternNode->condition.get());
        int elseJump = currentChunk->code.size();
        currentChunk->write(OpCode::OP_JUMP_IF_FALSE);
        currentChunk->write16(0);
        currentChunk->write(OpCode::OP_POP);
        compileExpression(ternNode->thenExpr.get());
        int endJump = currentChunk->code.size();
        currentChunk->write(OpCode::OP_JUMP);
        currentChunk->write16(0);
        currentChunk->patchJump(elseJump + 1);
        currentChunk->write(OpCode::OP_POP);
        compileExpression(ternNode->elseExpr.get());
        currentChunk->patchJump(endJump + 1);
    }
    else if (node->type == ASTNodeType::ARRAY) {
        ArrayNode* arrNode = static_cast<ArrayNode*>(node);
        for (auto& elem : arrNode->elements) {
            compileExpression(elem.get());
        }
        currentChunk->write(OpCode::OP_ARRAY);
        currentChunk->write(static_cast<uint8_t>(arrNode->elements.size()));
    }
    else if (node->type == ASTNodeType::INDEX) {
        IndexNode* idxNode = static_cast<IndexNode*>(node);
        compileExpression(idxNode->array.get());
        compileExpression(idxNode->index.get());
        currentChunk->write(OpCode::OP_INDEX_GET);
    }
    else if (node->type == ASTNodeType::IDENTIFIER) {
        IdentifierNode* idNode = static_cast<IdentifierNode*>(node);
        int localIdx = resolveLocal(idNode->name);
        if (localIdx != -1) {
            currentChunk->write(OpCode::OP_GET_LOCAL);
            currentChunk->write(localIdx);
        } else {
            int globalIdx = resolveGlobal(idNode->name);
            currentChunk->write(OpCode::OP_GET_GLOBAL);
            currentChunk->write(globalIdx);
        }
    }
    else if (node->type == ASTNodeType::BINARY_OP) {
        BinaryOpNode* binNode = static_cast<BinaryOpNode*>(node);
        
        auto optimized = optimizeConstantFolding(node);
        if (optimized) {
            compileExpression(optimized.get());
            return;
        }
        
        if (binNode->op == "and") {
            compileExpression(binNode->left.get());
            int jumpOffset = currentChunk->code.size();
            currentChunk->write(OpCode::OP_JUMP_IF_FALSE);
            currentChunk->write16(0);
            currentChunk->write(OpCode::OP_POP);
            compileExpression(binNode->right.get());
            currentChunk->patchJump(jumpOffset + 1);
        } else if (binNode->op == "or") {
            compileExpression(binNode->left.get());
            int jumpOffset = currentChunk->code.size();
            currentChunk->write(OpCode::OP_JUMP_IF_FALSE);
            currentChunk->write16(0);
            int endJump = currentChunk->code.size();
            currentChunk->write(OpCode::OP_JUMP);
            currentChunk->write16(0);
            currentChunk->patchJump(jumpOffset + 1);
            currentChunk->write(OpCode::OP_POP);
            compileExpression(binNode->right.get());
            currentChunk->patchJump(endJump + 1);
        } else {
            compileExpression(binNode->left.get());
            compileExpression(binNode->right.get());
            
            bool leftIsInt = binNode->left->type == ASTNodeType::NUMBER && 
                           static_cast<NumberNode*>(binNode->left.get())->value == 
                           static_cast<int>(static_cast<NumberNode*>(binNode->left.get())->value);
            bool rightIsInt = binNode->right->type == ASTNodeType::NUMBER && 
                            static_cast<NumberNode*>(binNode->right.get())->value == 
                            static_cast<int>(static_cast<NumberNode*>(binNode->right.get())->value);
            
            if (optimizationsEnabled && leftIsInt && rightIsInt) {
                if (binNode->op == "+") currentChunk->write(OpCode::OP_ADD_INT);
                else if (binNode->op == "-") currentChunk->write(OpCode::OP_SUB_INT);
                else if (binNode->op == "*") currentChunk->write(OpCode::OP_MUL_INT);
                else if (binNode->op == "/") currentChunk->write(OpCode::OP_DIVIDE);
                else if (binNode->op == "<") currentChunk->write(OpCode::OP_LESS);
                else if (binNode->op == ">") currentChunk->write(OpCode::OP_GREATER);
                else if (binNode->op == "<=") currentChunk->write(OpCode::OP_LESS_EQUAL);
                else if (binNode->op == ">=") currentChunk->write(OpCode::OP_GREATER_EQUAL);
                else if (binNode->op == "==") currentChunk->write(OpCode::OP_EQUAL);
                else if (binNode->op == "!=") currentChunk->write(OpCode::OP_NOT_EQUAL);
            } else {
                if (binNode->op == "+") currentChunk->write(OpCode::OP_ADD);
                else if (binNode->op == "-") currentChunk->write(OpCode::OP_SUBTRACT);
                else if (binNode->op == "*") currentChunk->write(OpCode::OP_MULTIPLY);
                else if (binNode->op == "/") currentChunk->write(OpCode::OP_DIVIDE);
                else if (binNode->op == "<") currentChunk->write(OpCode::OP_LESS);
                else if (binNode->op == ">") currentChunk->write(OpCode::OP_GREATER);
                else if (binNode->op == "<=") currentChunk->write(OpCode::OP_LESS_EQUAL);
                else if (binNode->op == ">=") currentChunk->write(OpCode::OP_GREATER_EQUAL);
                else if (binNode->op == "==") currentChunk->write(OpCode::OP_EQUAL);
                else if (binNode->op == "!=") currentChunk->write(OpCode::OP_NOT_EQUAL);
            }
        }
    }
    else if (node->type == ASTNodeType::UNARY_OP) {
        UnaryOpNode* unaryNode = static_cast<UnaryOpNode*>(node);
        compileExpression(unaryNode->operand.get());
        if (unaryNode->op == "-") currentChunk->write(OpCode::OP_NEGATE);
        else if (unaryNode->op == "!") currentChunk->write(OpCode::OP_NOT);
    }
    else if (node->type == ASTNodeType::FUNCTION_CALL) {
        FunctionCallNode* callNode = static_cast<FunctionCallNode*>(node);
        
        for (auto& arg : callNode->arguments) {
            compileExpression(arg.get());
        }
        
        if (callNode->name == "print") {
            currentChunk->write(OpCode::OP_PRINT);
            currentChunk->write(static_cast<uint8_t>(callNode->arguments.size()));
        } else if (callNode->name == "len" || callNode->name == "push" || callNode->name == "pop" ||
                   callNode->name == "sqrt" || callNode->name == "pow" || callNode->name == "abs" ||
                   callNode->name == "floor" || callNode->name == "ceil" || callNode->name == "sin" ||
                   callNode->name == "cos" || callNode->name == "tan" || callNode->name == "random" ||
                   callNode->name == "min" || callNode->name == "max" || callNode->name == "round" ||
                   callNode->name == "str" || callNode->name == "num" || callNode->name == "type" ||
                   callNode->name == "input" || callNode->name == "upper" || callNode->name == "lower" ||
                   callNode->name == "split" || callNode->name == "join" || callNode->name == "keys" ||
                   callNode->name == "values" || callNode->name == "read" || callNode->name == "write" ||
                   callNode->name == "append" || callNode->name == "exists" || callNode->name == "delete") {
            currentChunk->write(OpCode::OP_CALL);
            currentChunk->write(255);
            currentChunk->write(static_cast<uint8_t>(callNode->arguments.size()));
            int nameIdx = currentChunk->addConstant(Value(callNode->name));
            currentChunk->write(static_cast<uint8_t>(nameIdx));
        } else {
            if (functions.find(callNode->name) == functions.end()) {
                throw std::runtime_error("Undefined function: " + callNode->name);
            }
            int funcId = functions[callNode->name];
            currentChunk->write(OpCode::OP_CALL);
            currentChunk->write(static_cast<uint8_t>(funcId));
            currentChunk->write(static_cast<uint8_t>(callNode->arguments.size()));
        }
    }
}

void Compiler::compileStatement(ASTNode* node) {
    if (node->type == ASTNodeType::ASSIGNMENT) {
        AssignmentNode* assignNode = static_cast<AssignmentNode*>(node);
        compileExpression(assignNode->value.get());
        
        int localIdx = resolveLocal(assignNode->name);
        if (localIdx != -1) {
            currentChunk->write(OpCode::OP_SET_LOCAL);
            currentChunk->write(localIdx);
        } else {
            if (inFunction) {
                locals[assignNode->name] = localCount++;
                currentChunk->write(OpCode::OP_SET_LOCAL);
                currentChunk->write(locals[assignNode->name]);
            } else {
                int globalIdx = resolveGlobal(assignNode->name);
                currentChunk->write(OpCode::OP_SET_GLOBAL);
                currentChunk->write(globalIdx);
            }
        }
    }
    else if (node->type == ASTNodeType::FUNCTION_CALL) {
        FunctionCallNode* callNode = static_cast<FunctionCallNode*>(node);
        compileExpression(node);
        if (callNode->name != "print") {
            currentChunk->write(OpCode::OP_POP);
        }
    }
    else if (node->type == ASTNodeType::FUNCTION_DEF) {
        FunctionDefNode* funcNode = static_cast<FunctionDefNode*>(node);
        
        Function func;
        func.name = funcNode->name;
        func.arity = static_cast<int>(funcNode->params.size());
        func.params = funcNode->params;
        
        functions[funcNode->name] = static_cast<int>(functionTable.size());
        
        Chunk* prevChunk = currentChunk;
        currentChunk = &func.chunk;
        bool wasInFunction = inFunction;
        inFunction = true;
        
        beginScope();
        for (size_t i = 0; i < funcNode->params.size(); i++) {
            locals[funcNode->params[i]] = static_cast<int>(i);
            localCount++;
        }
        
        currentChunk->write(OpCode::OP_MAKEFRAME);
        currentChunk->write(localCount);
        
        if (funcNode->body->type == ASTNodeType::BLOCK) {
            BlockNode* block = static_cast<BlockNode*>(funcNode->body.get());
            for (auto& stmt : block->statements) {
                compileStatement(stmt.get());
            }
        }
        
        int constIdx = currentChunk->addConstant(Value(0.0));
        currentChunk->write(OpCode::OP_CONSTANT);
        currentChunk->write(constIdx);
        currentChunk->write(OpCode::OP_RET);
        
        endScope();
        inFunction = wasInFunction;
        currentChunk = prevChunk;
        
        functionTable.push_back(std::move(func));
    }
    else if (node->type == ASTNodeType::RETURN) {
        ReturnNode* retNode = static_cast<ReturnNode*>(node);
        compileExpression(retNode->value.get());
        currentChunk->write(OpCode::OP_RET);
    }
    else if (node->type == ASTNodeType::IF_STATEMENT) {
        IfStatementNode* ifNode = static_cast<IfStatementNode*>(node);
        
        if (optimizationsEnabled && ifNode->condition->type == ASTNodeType::BOOLEAN) {
            BooleanNode* boolNode = static_cast<BooleanNode*>(ifNode->condition.get());
            if (boolNode->value) {
                if (ifNode->thenBranch->type == ASTNodeType::BLOCK) {
                    BlockNode* block = static_cast<BlockNode*>(ifNode->thenBranch.get());
                    for (auto& stmt : block->statements) {
                        compileStatement(stmt.get());
                    }
                }
                return;
            } else {
                if (ifNode->elseBranch && ifNode->elseBranch->type == ASTNodeType::BLOCK) {
                    BlockNode* block = static_cast<BlockNode*>(ifNode->elseBranch.get());
                    for (auto& stmt : block->statements) {
                        compileStatement(stmt.get());
                    }
                }
                return;
            }
        }
        
        compileExpression(ifNode->condition.get());
        int thenJump = currentChunk->code.size();
        currentChunk->write(OpCode::OP_JUMP_IF_FALSE);
        currentChunk->write16(0);
        currentChunk->write(OpCode::OP_POP);
        
        if (ifNode->thenBranch->type == ASTNodeType::BLOCK) {
            BlockNode* block = static_cast<BlockNode*>(ifNode->thenBranch.get());
            for (auto& stmt : block->statements) {
                compileStatement(stmt.get());
            }
        }
        
        if (ifNode->elseBranch) {
            int elseJump = currentChunk->code.size();
            currentChunk->write(OpCode::OP_JUMP);
            currentChunk->write16(0);
            
            currentChunk->patchJump(thenJump + 1);
            currentChunk->write(OpCode::OP_POP);
            
            if (ifNode->elseBranch->type == ASTNodeType::BLOCK) {
                BlockNode* block = static_cast<BlockNode*>(ifNode->elseBranch.get());
                for (auto& stmt : block->statements) {
                    compileStatement(stmt.get());
                }
            }
            
            currentChunk->patchJump(elseJump + 1);
        } else {
            currentChunk->patchJump(thenJump + 1);
            currentChunk->write(OpCode::OP_POP);
        }
    }
    else if (node->type == ASTNodeType::WHILE_STATEMENT) {
        WhileStatementNode* whileNode = static_cast<WhileStatementNode*>(node);
        
        int loopStart = currentChunk->code.size();
        compileExpression(whileNode->condition.get());
        
        int exitJump = currentChunk->code.size();
        currentChunk->write(OpCode::OP_JUMP_IF_FALSE);
        currentChunk->write16(0);
        currentChunk->write(OpCode::OP_POP);
        
        if (whileNode->body->type == ASTNodeType::BLOCK) {
            BlockNode* block = static_cast<BlockNode*>(whileNode->body.get());
            for (auto& stmt : block->statements) {
                compileStatement(stmt.get());
            }
        }
        
        int offset = currentChunk->code.size() - loopStart + 3;
        currentChunk->write(OpCode::OP_JUMP);
        currentChunk->write16(-offset);
        
        currentChunk->patchJump(exitJump + 1);
        currentChunk->write(OpCode::OP_POP);
    }
    else if (node->type == ASTNodeType::FOR_STATEMENT) {
        ForStatementNode* forNode = static_cast<ForStatementNode*>(node);
        
        compileStatement(forNode->init.get());
        
        int loopStart = currentChunk->code.size();
        compileExpression(forNode->condition.get());
        
        int exitJump = currentChunk->code.size();
        currentChunk->write(OpCode::OP_JUMP_IF_FALSE);
        currentChunk->write16(0);
        currentChunk->write(OpCode::OP_POP);
        
        if (forNode->body->type == ASTNodeType::BLOCK) {
            BlockNode* block = static_cast<BlockNode*>(forNode->body.get());
            for (auto& stmt : block->statements) {
                compileStatement(stmt.get());
            }
        }
        
        compileStatement(forNode->increment.get());
        
        int offset = currentChunk->code.size() - loopStart + 3;
        currentChunk->write(OpCode::OP_JUMP);
        currentChunk->write16(-offset);
        
        currentChunk->patchJump(exitJump + 1);
        currentChunk->write(OpCode::OP_POP);
    }
    else if (node->type == ASTNodeType::USE_STATEMENT) {
        UseStatementNode* useNode = static_cast<UseStatementNode*>(node);
        loadStandardLibrary(useNode->library);
    }
    else if (node->type == ASTNodeType::BREAK_STATEMENT) {
        currentChunk->write(OpCode::OP_BREAK);
    }
    else if (node->type == ASTNodeType::CONTINUE_STATEMENT) {
        currentChunk->write(OpCode::OP_CONTINUE);
    }
}

void Compiler::saveBytecode(const std::string& filename, const Chunk& chunk) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return;
    
    // Magic number
    file.write("ZSC", 3);
    
    // Version
    uint8_t version = 3;
    file.write(reinterpret_cast<const char*>(&version), 1);
    
    // Code size
    uint32_t codeSize = static_cast<uint32_t>(chunk.code.size());
    file.write(reinterpret_cast<const char*>(&codeSize), 4);
    
    // Code
    file.write(reinterpret_cast<const char*>(chunk.code.data()), codeSize);
    
    // Constants size
    uint32_t constSize = static_cast<uint32_t>(chunk.constants.size());
    file.write(reinterpret_cast<const char*>(&constSize), 4);
    
    // Constants
    for (const auto& val : chunk.constants) {
        uint8_t type = static_cast<uint8_t>(val.type);
        file.write(reinterpret_cast<const char*>(&type), 1);
        
        if (val.type == Value::NUMBER) {
            file.write(reinterpret_cast<const char*>(&val.number), sizeof(double));
        } else if (val.type == Value::STRING) {
            uint32_t len = static_cast<uint32_t>(val.string.length());
            file.write(reinterpret_cast<const char*>(&len), 4);
            file.write(val.string.c_str(), len);
        } else if (val.type == Value::BOOLEAN) {
            file.write(reinterpret_cast<const char*>(&val.boolean), 1);
        }
    }
    
    // Functions size
    uint32_t funcSize = static_cast<uint32_t>(functionTable.size());
    file.write(reinterpret_cast<const char*>(&funcSize), 4);
    
    file.close();
}

Chunk Compiler::loadBytecode(const std::string& filename) {
    Chunk chunk;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return chunk;
    
    // Verify magic number
    char magic[3];
    file.read(magic, 3);
    if (magic[0] != 'Z' || magic[1] != 'S' || magic[2] != 'C') return chunk;
    
    // Version
    uint8_t version;
    file.read(reinterpret_cast<char*>(&version), 1);
    
    // Code
    uint32_t codeSize;
    file.read(reinterpret_cast<char*>(&codeSize), 4);
    chunk.code.resize(codeSize);
    file.read(reinterpret_cast<char*>(chunk.code.data()), codeSize);
    
    // Constants
    uint32_t constSize;
    file.read(reinterpret_cast<char*>(&constSize), 4);
    
    for (uint32_t i = 0; i < constSize; i++) {
        uint8_t type;
        file.read(reinterpret_cast<char*>(&type), 1);
        
        if (type == static_cast<uint8_t>(Value::NUMBER)) {
            double num;
            file.read(reinterpret_cast<char*>(&num), sizeof(double));
            chunk.constants.push_back(Value(num));
        } else if (type == static_cast<uint8_t>(Value::STRING)) {
            uint32_t len;
            file.read(reinterpret_cast<char*>(&len), 4);
            std::string str(len, '\0');
            file.read(&str[0], len);
            chunk.constants.push_back(Value(str));
        } else if (type == static_cast<uint8_t>(Value::BOOLEAN)) {
            bool b;
            file.read(reinterpret_cast<char*>(&b), 1);
            chunk.constants.push_back(Value(b));
        }
    }
    
    file.close();
    return chunk;
}

void Compiler::peepholeOptimize(Chunk& chunk) {
    return;
}

Chunk Compiler::compile(ProgramNode* program) {
    Chunk mainChunk;
    currentChunk = &mainChunk;
    
    for (auto& stmt : program->statements) {
        compileStatement(stmt.get());
    }
    
    currentChunk->write(OpCode::OP_HALT);
    return mainChunk;
}
