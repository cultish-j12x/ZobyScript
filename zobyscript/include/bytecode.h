#ifndef BYTECODE_H
#define BYTECODE_H

#include <vector>
#include <string>
#include <map>
#include <cstdint>

enum class OpCode : uint8_t {
    OP_CONSTANT,
    OP_STRING,
    OP_TRUE,
    OP_FALSE,
    OP_NULL,
    OP_ARRAY,
    OP_HASHMAP,
    OP_INDEX_GET,
    OP_INDEX_SET,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_NOT,
    OP_LESS,
    OP_GREATER,
    OP_LESS_EQUAL,
    OP_GREATER_EQUAL,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_AND,
    OP_OR,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_BREAK,
    OP_CONTINUE,
    OP_CALL,
    OP_RET,
    OP_MAKEFRAME,
    OP_POPFRAME,
    OP_PRINT,
    OP_POP,
    OP_HALT,
    // Fast path opcodes
    OP_ADD_INT,
    OP_SUB_INT,
    OP_MUL_INT,
    OP_CONSTANT_0,
    OP_CONSTANT_1,
    OP_GET_GLOBAL_CACHED,
    OP_SET_GLOBAL_CACHED
};

struct Value {
    enum Type { NUMBER, STRING, BOOLEAN, ARRAY, HASHMAP, NULLVAL } type;
    double number;
    std::string string;
    bool boolean;
    std::vector<Value>* array;
    std::map<std::string, Value>* hashmap;
    
    Value() : type(NUMBER), number(0), array(nullptr), hashmap(nullptr) {}
    Value(double n) : type(NUMBER), number(n), array(nullptr), hashmap(nullptr) {}
    Value(const std::string& s) : type(STRING), string(s), array(nullptr), hashmap(nullptr) {}
    Value(bool b) : type(BOOLEAN), boolean(b), array(nullptr), hashmap(nullptr) {}
    Value(std::vector<Value>* arr) : type(ARRAY), number(0), array(arr), hashmap(nullptr) {}
    Value(std::map<std::string, Value>* hm) : type(HASHMAP), number(0), array(nullptr), hashmap(hm) {}
    static Value Null() { Value v; v.type = NULLVAL; v.array = nullptr; v.hashmap = nullptr; return v; }
    
    ~Value() {
        if (type == ARRAY && array) delete array;
        if (type == HASHMAP && hashmap) delete hashmap;
    }
    
    Value(const Value& other) : type(other.type), number(other.number), string(other.string), boolean(other.boolean) {
        if (other.type == ARRAY && other.array) {
            array = new std::vector<Value>(*other.array);
        } else {
            array = nullptr;
        }
        if (other.type == HASHMAP && other.hashmap) {
            hashmap = new std::map<std::string, Value>(*other.hashmap);
        } else {
            hashmap = nullptr;
        }
    }
    
    Value& operator=(const Value& other) {
        if (this != &other) {
            if (type == ARRAY && array) delete array;
            if (type == HASHMAP && hashmap) delete hashmap;
            type = other.type;
            number = other.number;
            string = other.string;
            boolean = other.boolean;
            if (other.type == ARRAY && other.array) {
                array = new std::vector<Value>(*other.array);
            } else {
                array = nullptr;
            }
            if (other.type == HASHMAP && other.hashmap) {
                hashmap = new std::map<std::string, Value>(*other.hashmap);
            } else {
                hashmap = nullptr;
            }
        }
        return *this;
    }
};

struct Chunk {
    std::vector<uint8_t> code;
    std::vector<Value> constants;
    
    void write(uint8_t byte) { code.push_back(byte); }
    void write(OpCode op) { code.push_back(static_cast<uint8_t>(op)); }
    void write16(int value) {
        code.push_back((value >> 8) & 0xFF);
        code.push_back(value & 0xFF);
    }
    int addConstant(const Value& value) {
        constants.push_back(value);
        return static_cast<int>(constants.size() - 1);
    }
    void patchJump(int offset) {
        int jump = code.size() - offset - 2;
        code[offset] = (jump >> 8) & 0xFF;
        code[offset + 1] = jump & 0xFF;
    }
};

#endif
