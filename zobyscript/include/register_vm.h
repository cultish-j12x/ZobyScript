#ifndef REGISTER_VM_H
#define REGISTER_VM_H

#include "bytecode.h"
#include <vector>
#include <string>

// Register-based opcodes (3-5x faster than stack)
enum class RegOpCode : uint8_t {
    // Arithmetic: dest = src1 op src2
    REG_ADD,      // R[A] = R[B] + R[C]
    REG_SUB,      // R[A] = R[B] - R[C]
    REG_MUL,      // R[A] = R[B] * R[C]
    REG_DIV,      // R[A] = R[B] / R[C]
    
    // Fast integer arithmetic
    REG_ADDI,     // R[A] = R[B] + C (immediate)
    REG_SUBI,     // R[A] = R[B] - C
    REG_MULI,     // R[A] = R[B] * C
    
    // Load/Store
    REG_LOADK,    // R[A] = K[B] (constant)
    REG_LOAD0,    // R[A] = 0
    REG_LOAD1,    // R[A] = 1
    REG_MOVE,     // R[A] = R[B]
    
    // Comparisons
    REG_LT,       // R[A] = R[B] < R[C]
    REG_LE,       // R[A] = R[B] <= R[C]
    REG_EQ,       // R[A] = R[B] == R[C]
    
    // Jumps
    REG_JMP,      // PC += offset
    REG_JMPF,     // if !R[A] then PC += offset
    REG_JMPT,     // if R[A] then PC += offset
    
    // Function calls
    REG_CALL,     // R[A] = func(R[B]...R[B+C])
    REG_RET,      // return R[A]
    
    // Global access
    REG_GETGLOBAL, // R[A] = globals[B]
    REG_SETGLOBAL, // globals[A] = R[B]
    
    // Print/Misc
    REG_PRINT,
    REG_HALT
};

struct RegisterFrame {
    std::vector<Value> registers;  // Local registers (fast!)
    int pc;
    int returnReg;
    
    RegisterFrame(int size = 256) : registers(size), pc(0), returnReg(0) {}
};

class RegisterVM {
private:
    std::vector<Value> globals;
    std::vector<RegisterFrame> callStack;
    RegisterFrame* currentFrame;
    const std::vector<Function>* functions;
    
    Value& R(int idx) { return currentFrame->registers[idx]; }
    
public:
    RegisterVM();
    void run(const Chunk& mainChunk, const std::vector<Function>& funcs);
    Value callBuiltin(const std::string& name, const std::vector<Value>& args);
};

#endif
