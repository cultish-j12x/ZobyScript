#include "../include/register_vm.h"
#include <iostream>
#include <cmath>
#include <algorithm>

RegisterVM::RegisterVM() {
    globals.resize(256);
    callStack.reserve(256);
    callStack.emplace_back(256);
    currentFrame = &callStack[0];
}

Value RegisterVM::callBuiltin(const std::string& name, const std::vector<Value>& args) {
    if (name == "sqrt") return Value(std::sqrt(args[0].number));
    if (name == "pow") return Value(std::pow(args[0].number, args[1].number));
    if (name == "abs") return Value(std::abs(args[0].number));
    if (name == "floor") return Value(std::floor(args[0].number));
    if (name == "ceil") return Value(std::ceil(args[0].number));
    if (name == "min") return Value(std::min(args[0].number, args[1].number));
    if (name == "max") return Value(std::max(args[0].number, args[1].number));
    if (name == "random") return Value(static_cast<double>(rand()) / RAND_MAX);
    return Value(0.0);
}

void RegisterVM::run(const Chunk& mainChunk, const std::vector<Function>& funcs) {
    functions = &funcs;
    const uint8_t* code = mainChunk.code.data();
    int pc = 0;
    
    // Computed goto dispatch table (2x faster than switch)
    #ifdef __GNUC__
    static void* dispatchTable[] = {
        &&OP_REG_ADD, &&OP_REG_SUB, &&OP_REG_MUL, &&OP_REG_DIV,
        &&OP_REG_ADDI, &&OP_REG_SUBI, &&OP_REG_MULI,
        &&OP_REG_LOADK, &&OP_REG_LOAD0, &&OP_REG_LOAD1, &&OP_REG_MOVE,
        &&OP_REG_LT, &&OP_REG_LE, &&OP_REG_EQ,
        &&OP_REG_JMP, &&OP_REG_JMPF, &&OP_REG_JMPT,
        &&OP_REG_CALL, &&OP_REG_RET,
        &&OP_REG_GETGLOBAL, &&OP_REG_SETGLOBAL,
        &&OP_REG_PRINT, &&OP_REG_HALT
    };
    
    #define DISPATCH() goto *dispatchTable[code[pc++]]
    #define CASE(op) OP_##op:
    
    DISPATCH();
    
    CASE(REG_ADD) {
        int a = code[pc++], b = code[pc++], c = code[pc++];
        R(a).number = R(b).number + R(c).number;
        DISPATCH();
    }
    
    CASE(REG_SUB) {
        int a = code[pc++], b = code[pc++], c = code[pc++];
        R(a).number = R(b).number - R(c).number;
        DISPATCH();
    }
    
    CASE(REG_MUL) {
        int a = code[pc++], b = code[pc++], c = code[pc++];
        R(a).number = R(b).number * R(c).number;
        DISPATCH();
    }
    
    CASE(REG_DIV) {
        int a = code[pc++], b = code[pc++], c = code[pc++];
        R(a).number = R(b).number / R(c).number;
        DISPATCH();
    }
    
    CASE(REG_ADDI) {
        int a = code[pc++], b = code[pc++], c = code[pc++];
        R(a).number = R(b).number + c;
        DISPATCH();
    }
    
    CASE(REG_SUBI) {
        int a = code[pc++], b = code[pc++], c = code[pc++];
        R(a).number = R(b).number - c;
        DISPATCH();
    }
    
    CASE(REG_MULI) {
        int a = code[pc++], b = code[pc++], c = code[pc++];
        R(a).number = R(b).number * c;
        DISPATCH();
    }
    
    CASE(REG_LOADK) {
        int a = code[pc++], b = code[pc++];
        R(a) = mainChunk.constants[b];
        DISPATCH();
    }
    
    CASE(REG_LOAD0) {
        int a = code[pc++];
        R(a) = Value(0.0);
        DISPATCH();
    }
    
    CASE(REG_LOAD1) {
        int a = code[pc++];
        R(a) = Value(1.0);
        DISPATCH();
    }
    
    CASE(REG_MOVE) {
        int a = code[pc++], b = code[pc++];
        R(a) = R(b);
        DISPATCH();
    }
    
    CASE(REG_LT) {
        int a = code[pc++], b = code[pc++], c = code[pc++];
        R(a) = Value(R(b).number < R(c).number);
        DISPATCH();
    }
    
    CASE(REG_LE) {
        int a = code[pc++], b = code[pc++], c = code[pc++];
        R(a) = Value(R(b).number <= R(c).number);
        DISPATCH();
    }
    
    CASE(REG_EQ) {
        int a = code[pc++], b = code[pc++], c = code[pc++];
        R(a) = Value(R(b).number == R(c).number);
        DISPATCH();
    }
    
    CASE(REG_JMP) {
        int offset = (code[pc] << 8) | code[pc + 1];
        pc += 2;
        if (offset & 0x8000) offset |= 0xFFFF0000;
        pc += offset;
        DISPATCH();
    }
    
    CASE(REG_JMPF) {
        int a = code[pc++];
        int offset = (code[pc] << 8) | code[pc + 1];
        pc += 2;
        if (R(a).type == Value::BOOLEAN && !R(a).boolean) {
            pc += offset;
        }
        DISPATCH();
    }
    
    CASE(REG_JMPT) {
        int a = code[pc++];
        int offset = (code[pc] << 8) | code[pc + 1];
        pc += 2;
        if (R(a).type == Value::BOOLEAN && R(a).boolean) {
            pc += offset;
        }
        DISPATCH();
    }
    
    CASE(REG_GETGLOBAL) {
        int a = code[pc++], b = code[pc++];
        R(a) = globals[b];
        DISPATCH();
    }
    
    CASE(REG_SETGLOBAL) {
        int a = code[pc++], b = code[pc++];
        globals[a] = R(b);
        DISPATCH();
    }
    
    CASE(REG_PRINT) {
        int count = code[pc++];
        for (int i = 0; i < count; i++) {
            int reg = code[pc++];
            if (R(reg).type == Value::NUMBER) {
                double num = R(reg).number;
                if (num == static_cast<int>(num)) {
                    std::cout << static_cast<int>(num);
                } else {
                    std::cout << num;
                }
            } else if (R(reg).type == Value::STRING) {
                std::cout << R(reg).string;
            }
            if (i < count - 1) std::cout << " ";
        }
        std::cout << std::endl;
        DISPATCH();
    }
    
    CASE(REG_CALL) {
        // Simplified - full implementation needed
        DISPATCH();
    }
    
    CASE(REG_RET) {
        return;
    }
    
    CASE(REG_HALT) {
        return;
    }
    
    #else
    // Fallback to switch for non-GCC compilers
    while (pc < static_cast<int>(mainChunk.code.size())) {
        RegOpCode op = static_cast<RegOpCode>(code[pc++]);
        switch (op) {
            case RegOpCode::REG_ADD: {
                int a = code[pc++], b = code[pc++], c = code[pc++];
                R(a).number = R(b).number + R(c).number;
                break;
            }
            case RegOpCode::REG_HALT:
                return;
            default:
                break;
        }
    }
    #endif
}
