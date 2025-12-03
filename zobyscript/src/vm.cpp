#include "../include/vm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <cstdio>

VM::VM() : ip(0), bp(0), optimizationsEnabled(true) {
    globals.resize(256);
    globalCaches.resize(256);
}

void VM::push(const Value& value) {
    stack.push_back(value);
}

Value VM::pop() {
    if (stack.empty()) throw std::runtime_error("Stack underflow");
    Value value = stack.back();
    stack.pop_back();
    return value;
}

Value VM::peek(int offset) {
    return stack[stack.size() - 1 - offset];
}

bool VM::isTruthy(const Value& value) {
    if (value.type == Value::BOOLEAN) return value.boolean;
    if (value.type == Value::NUMBER) return value.number != 0;
    if (value.type == Value::STRING) return !value.string.empty();
    if (value.type == Value::ARRAY) return value.array && !value.array->empty();
    return false;
}

Value VM::callBuiltin(const std::string& name, const std::vector<Value>& args) {
    if (name == "len") {
        if (args[0].type == Value::ARRAY) return Value(static_cast<double>(args[0].array->size()));
        if (args[0].type == Value::STRING) return Value(static_cast<double>(args[0].string.length()));
        return Value(0.0);
    }
    if (name == "push" && args.size() == 2 && args[0].type == Value::ARRAY) {
        args[0].array->push_back(args[1]);
        return args[0];
    }
    if (name == "pop" && args[0].type == Value::ARRAY && !args[0].array->empty()) {
        Value val = args[0].array->back();
        args[0].array->pop_back();
        return val;
    }
    if (name == "sqrt") return Value(std::sqrt(args[0].number));
    if (name == "pow" && args.size() == 2) return Value(std::pow(args[0].number, args[1].number));
    if (name == "abs") return Value(std::abs(args[0].number));
    if (name == "floor") return Value(std::floor(args[0].number));
    if (name == "ceil") return Value(std::ceil(args[0].number));
    if (name == "sin") return Value(std::sin(args[0].number));
    if (name == "cos") return Value(std::cos(args[0].number));
    if (name == "tan") return Value(std::tan(args[0].number));
    if (name == "random") return Value(static_cast<double>(rand()) / RAND_MAX);
    if (name == "min" && args.size() == 2) return Value(std::min(args[0].number, args[1].number));
    if (name == "max" && args.size() == 2) return Value(std::max(args[0].number, args[1].number));
    if (name == "round") return Value(std::round(args[0].number));
    if (name == "str" && args.size() == 1) {
        if (args[0].type == Value::NUMBER) {
            return Value(std::to_string(static_cast<int>(args[0].number)));
        } else if (args[0].type == Value::BOOLEAN) {
            return Value(args[0].boolean ? "true" : "false");
        }
        return args[0];
    }
    if (name == "num" && args.size() == 1 && args[0].type == Value::STRING) {
        try {
            return Value(std::stod(args[0].string));
        } catch (...) {
            return Value(0.0);
        }
    }
    if (name == "type" && args.size() == 1) {
        if (args[0].type == Value::NUMBER) return Value("number");
        if (args[0].type == Value::STRING) return Value("string");
        if (args[0].type == Value::BOOLEAN) return Value("boolean");
        if (args[0].type == Value::ARRAY) return Value("array");
    }
    if (name == "input" && args.size() >= 1 && args[0].type == Value::STRING) {
        std::cout << args[0].string;
        std::string input;
        std::getline(std::cin, input);
        return Value(input);
    }
    if (name == "upper" && args.size() == 1 && args[0].type == Value::STRING) {
        std::string result = args[0].string;
        for (char& c : result) c = std::toupper(c);
        return Value(result);
    }
    if (name == "lower" && args.size() == 1 && args[0].type == Value::STRING) {
        std::string result = args[0].string;
        for (char& c : result) c = std::tolower(c);
        return Value(result);
    }
    if (name == "split" && args.size() == 2 && args[0].type == Value::STRING && args[1].type == Value::STRING) {
        auto* arr = new std::vector<Value>();
        std::string str = args[0].string;
        std::string delim = args[1].string;
        size_t pos = 0;
        while ((pos = str.find(delim)) != std::string::npos) {
            arr->push_back(Value(str.substr(0, pos)));
            str.erase(0, pos + delim.length());
        }
        if (!str.empty()) arr->push_back(Value(str));
        return Value(arr);
    }
    if (name == "join" && args.size() == 2 && args[0].type == Value::ARRAY && args[1].type == Value::STRING) {
        std::string result;
        for (size_t i = 0; i < args[0].array->size(); i++) {
            if ((*args[0].array)[i].type == Value::STRING) {
                result += (*args[0].array)[i].string;
            }
            if (i < args[0].array->size() - 1) result += args[1].string;
        }
        return Value(result);
    }
    if (name == "keys" && args.size() == 1 && args[0].type == Value::HASHMAP) {
        auto* arr = new std::vector<Value>();
        for (const auto& pair : *args[0].hashmap) {
            arr->push_back(Value(pair.first));
        }
        return Value(arr);
    }
    if (name == "values" && args.size() == 1 && args[0].type == Value::HASHMAP) {
        auto* arr = new std::vector<Value>();
        for (const auto& pair : *args[0].hashmap) {
            arr->push_back(pair.second);
        }
        return Value(arr);
    }
    if (name == "read" && args.size() == 1 && args[0].type == Value::STRING) {
        std::ifstream file(args[0].string);
        if (!file.is_open()) return Value::Null();
        std::stringstream buffer;
        buffer << file.rdbuf();
        return Value(buffer.str());
    }
    if (name == "write" && args.size() == 2 && args[0].type == Value::STRING && args[1].type == Value::STRING) {
        std::ofstream file(args[0].string);
        if (!file.is_open()) return Value(false);
        file << args[1].string;
        file.close();
        return Value(true);
    }
    if (name == "append" && args.size() == 2 && args[0].type == Value::STRING && args[1].type == Value::STRING) {
        std::ofstream file(args[0].string, std::ios::app);
        if (!file.is_open()) return Value(false);
        file << args[1].string;
        file.close();
        return Value(true);
    }
    if (name == "exists" && args.size() == 1 && args[0].type == Value::STRING) {
        std::ifstream file(args[0].string);
        return Value(file.good());
    }
    if (name == "delete" && args.size() == 1 && args[0].type == Value::STRING) {
        return Value(std::remove(args[0].string.c_str()) == 0);
    }
    return Value(0.0);
}

void VM::executeChunk(const Chunk& chunk) {
    ip = 0;
    
    while (ip < chunk.code.size()) {
        OpCode op = static_cast<OpCode>(chunk.code[ip++]);
        
        switch (op) {
            case OpCode::OP_CONSTANT: {
                int constIdx = chunk.code[ip++];
                push(chunk.constants[constIdx]);
                break;
            }
            case OpCode::OP_CONSTANT_0: {
                push(Value(0.0));
                break;
            }
            case OpCode::OP_CONSTANT_1: {
                push(Value(1.0));
                break;
            }
            case OpCode::OP_STRING: {
                int constIdx = chunk.code[ip++];
                push(chunk.constants[constIdx]);
                break;
            }
            case OpCode::OP_TRUE: {
                push(Value(true));
                break;
            }
            case OpCode::OP_FALSE: {
                push(Value(false));
                break;
            }
            case OpCode::OP_ARRAY: {
                int size = chunk.code[ip++];
                auto* arr = new std::vector<Value>();
                for (int i = size - 1; i >= 0; i--) {
                    arr->insert(arr->begin(), pop());
                }
                push(Value(arr));
                break;
            }
            case OpCode::OP_INDEX_GET: {
                Value index = pop();
                Value array = pop();
                if (array.type == Value::HASHMAP && index.type == Value::STRING) {
                    if (array.hashmap->find(index.string) != array.hashmap->end()) {
                        push((*array.hashmap)[index.string]);
                    } else {
                        push(Value::Null());
                    }
                } else if (array.type == Value::ARRAY && index.type == Value::NUMBER) {
                    int idx = static_cast<int>(index.number);
                    if (idx >= 0 && idx < static_cast<int>(array.array->size())) {
                        push((*array.array)[idx]);
                    } else {
                        push(Value(0.0));
                    }
                } else if (array.type == Value::STRING && index.type == Value::NUMBER) {
                    int idx = static_cast<int>(index.number);
                    if (idx >= 0 && idx < static_cast<int>(array.string.length())) {
                        push(Value(std::string(1, array.string[idx])));
                    } else {
                        push(Value(""));
                    }
                }
                break;
            }
            case OpCode::OP_INDEX_SET: {
                Value value = pop();
                Value index = pop();
                Value array = pop();
                if (array.type == Value::ARRAY && index.type == Value::NUMBER) {
                    int idx = static_cast<int>(index.number);
                    if (idx >= 0 && idx < static_cast<int>(array.array->size())) {
                        (*array.array)[idx] = value;
                    }
                }
                push(array);
                break;
            }
            case OpCode::OP_ADD: {
                Value b = pop();
                Value a = pop();
                if (a.type == Value::STRING || b.type == Value::STRING) {
                    std::string result;
                    if (a.type == Value::STRING) result += a.string;
                    else if (a.type == Value::NUMBER) result += std::to_string(static_cast<int>(a.number));
                    if (b.type == Value::STRING) result += b.string;
                    else if (b.type == Value::NUMBER) result += std::to_string(static_cast<int>(b.number));
                    push(Value(result));
                } else {
                    push(Value(a.number + b.number));
                }
                break;
            }
            case OpCode::OP_ADD_INT: {
                Value b = pop();
                Value a = pop();
                int result = static_cast<int>(a.number) + static_cast<int>(b.number);
                push(Value(static_cast<double>(result)));
                break;
            }
            case OpCode::OP_SUBTRACT: {
                Value b = pop();
                Value a = pop();
                push(Value(a.number - b.number));
                break;
            }
            case OpCode::OP_SUB_INT: {
                Value b = pop();
                Value a = pop();
                int result = static_cast<int>(a.number) - static_cast<int>(b.number);
                push(Value(static_cast<double>(result)));
                break;
            }
            case OpCode::OP_MULTIPLY: {
                Value b = pop();
                Value a = pop();
                push(Value(a.number * b.number));
                break;
            }
            case OpCode::OP_MUL_INT: {
                Value b = pop();
                Value a = pop();
                int result = static_cast<int>(a.number) * static_cast<int>(b.number);
                push(Value(static_cast<double>(result)));
                break;
            }
            case OpCode::OP_DIVIDE: {
                Value b = pop();
                Value a = pop();
                if (b.number == 0) throw std::runtime_error("Division by zero");
                push(Value(a.number / b.number));
                break;
            }
            case OpCode::OP_NEGATE: {
                Value a = pop();
                push(Value(-a.number));
                break;
            }
            case OpCode::OP_NOT: {
                Value a = pop();
                push(Value(!isTruthy(a)));
                break;
            }
            case OpCode::OP_LESS: {
                Value b = pop();
                Value a = pop();
                push(Value(a.number < b.number));
                break;
            }
            case OpCode::OP_GREATER: {
                Value b = pop();
                Value a = pop();
                push(Value(a.number > b.number));
                break;
            }
            case OpCode::OP_LESS_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(Value(a.number <= b.number));
                break;
            }
            case OpCode::OP_GREATER_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(Value(a.number >= b.number));
                break;
            }
            case OpCode::OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                if (a.type != b.type) {
                    push(Value(false));
                } else if (a.type == Value::NUMBER) {
                    push(Value(a.number == b.number));
                } else if (a.type == Value::STRING) {
                    push(Value(a.string == b.string));
                } else {
                    push(Value(a.boolean == b.boolean));
                }
                break;
            }
            case OpCode::OP_NOT_EQUAL: {
                Value b = pop();
                Value a = pop();
                if (a.type != b.type) {
                    push(Value(true));
                } else if (a.type == Value::NUMBER) {
                    push(Value(a.number != b.number));
                } else if (a.type == Value::STRING) {
                    push(Value(a.string != b.string));
                } else {
                    push(Value(a.boolean != b.boolean));
                }
                break;
            }
            case OpCode::OP_GET_GLOBAL: {
                int globalIdx = chunk.code[ip++];
                if (globalIdx < static_cast<int>(globals.size())) {
                    push(globals[globalIdx]);
                } else {
                    push(Value(0.0));
                }
                break;
            }
            case OpCode::OP_GET_GLOBAL_CACHED: {
                int cacheIdx = chunk.code[ip++];
                if (cacheIdx < static_cast<int>(globalCaches.size()) && globalCaches[cacheIdx].valid) {
                    push(globalCaches[cacheIdx].cachedValue);
                } else {
                    int globalIdx = chunk.code[ip++];
                    if (globalIdx < static_cast<int>(globals.size())) {
                        push(globals[globalIdx]);
                        if (cacheIdx < static_cast<int>(globalCaches.size())) {
                            globalCaches[cacheIdx].cachedValue = globals[globalIdx];
                            globalCaches[cacheIdx].valid = true;
                        }
                    } else {
                        push(Value(0.0));
                    }
                }
                break;
            }
            case OpCode::OP_SET_GLOBAL: {
                int globalIdx = chunk.code[ip++];
                if (globalIdx >= static_cast<int>(globals.size())) {
                    globals.resize(globalIdx + 1);
                }
                globals[globalIdx] = peek(0);
                if (globalIdx < static_cast<int>(globalCaches.size())) {
                    globalCaches[globalIdx].valid = false;
                }
                break;
            }
            case OpCode::OP_SET_GLOBAL_CACHED: {
                int cacheIdx = chunk.code[ip++];
                int globalIdx = chunk.code[ip++];
                if (globalIdx >= static_cast<int>(globals.size())) {
                    globals.resize(globalIdx + 1);
                }
                globals[globalIdx] = peek(0);
                if (cacheIdx < static_cast<int>(globalCaches.size())) {
                    globalCaches[cacheIdx].cachedValue = peek(0);
                    globalCaches[cacheIdx].valid = true;
                }
                break;
            }
            case OpCode::OP_GET_LOCAL: {
                int localIdx = chunk.code[ip++];
                push(stack[static_cast<size_t>(bp) + localIdx]);
                break;
            }
            case OpCode::OP_SET_LOCAL: {
                int localIdx = chunk.code[ip++];
                if (static_cast<size_t>(bp + localIdx) >= stack.size()) {
                    stack.resize(static_cast<size_t>(bp + localIdx + 1));
                }
                stack[static_cast<size_t>(bp) + localIdx] = peek(0);
                break;
            }
            case OpCode::OP_JUMP: {
                int offset = (chunk.code[ip] << 8) | chunk.code[ip + 1];
                ip += 2;
                if (offset & 0x8000) {
                    offset |= 0xFFFF0000;
                }
                ip += offset;
                break;
            }
            case OpCode::OP_JUMP_IF_FALSE: {
                int offset = (chunk.code[ip] << 8) | chunk.code[ip + 1];
                ip += 2;
                if (!isTruthy(peek(0))) {
                    ip += offset;
                }
                break;
            }
            case OpCode::OP_BREAK: {
                throw std::runtime_error("break outside loop");
            }
            case OpCode::OP_CONTINUE: {
                throw std::runtime_error("continue outside loop");
            }
            case OpCode::OP_CALL: {
                int funcId = chunk.code[ip++];
                int argc = chunk.code[ip++];
                
                if (funcId == 255) {
                    int nameIdx = chunk.code[ip++];
                    std::string funcName = chunk.constants[nameIdx].string;
                    std::vector<Value> args;
                    for (int i = 0; i < argc; i++) {
                        args.insert(args.begin(), pop());
                    }
                    Value result = callBuiltin(funcName, args);
                    push(result);
                    break;
                }
                
                CallFrame frame;
                frame.returnIP = ip;
                frame.basePointer = bp;
                frame.functionId = funcId;
                callStack.push_back(frame);
                
                bp = static_cast<int>(stack.size()) - argc;
                
                executeChunk((*functions)[funcId].chunk);
                
                Value retVal = pop();
                
                while (static_cast<int>(stack.size()) > bp) {
                    stack.pop_back();
                }
                
                CallFrame lastFrame = callStack.back();
                callStack.pop_back();
                bp = lastFrame.basePointer;
                ip = lastFrame.returnIP;
                
                push(retVal);
                break;
            }
            case OpCode::OP_RET: {
                return;
            }
            case OpCode::OP_MAKEFRAME: {
                int localCount = chunk.code[ip++];
                break;
            }
            case OpCode::OP_POPFRAME: {
                break;
            }
            case OpCode::OP_PRINT: {
                int argc = chunk.code[ip++];
                for (int i = argc - 1; i >= 0; i--) {
                    Value val = peek(i);
                    if (val.type == Value::NUMBER) {
                        double num = val.number;
                        if (num == static_cast<int>(num)) {
                            std::cout << static_cast<int>(num);
                        } else {
                            std::cout << num;
                        }
                    } else if (val.type == Value::STRING) {
                        std::cout << val.string;
                    } else if (val.type == Value::BOOLEAN) {
                        std::cout << (val.boolean ? "true" : "false");
                    } else if (val.type == Value::ARRAY) {
                        std::cout << "[";
                        for (size_t j = 0; j < val.array->size(); j++) {
                            Value elem = (*val.array)[j];
                            if (elem.type == Value::NUMBER) {
                                double num = elem.number;
                                if (num == static_cast<int>(num)) {
                                    std::cout << static_cast<int>(num);
                                } else {
                                    std::cout << num;
                                }
                            } else if (elem.type == Value::STRING) {
                                std::cout << "\"" << elem.string << "\"";
                            } else if (elem.type == Value::BOOLEAN) {
                                std::cout << (elem.boolean ? "true" : "false");
                            }
                            if (j < val.array->size() - 1) std::cout << ", ";
                        }
                        std::cout << "]";
                    }
                    if (i > 0) std::cout << " ";
                }
                std::cout << std::endl;
                for (int i = 0; i < argc; i++) pop();
                break;
            }
            case OpCode::OP_POP: {
                pop();
                break;
            }
            case OpCode::OP_HALT: {
                return;
            }
        }
    }
}

void VM::run(const Chunk& mainChunk, const std::vector<Function>& funcs) {
    functions = &funcs;
    executeChunk(mainChunk);
}
