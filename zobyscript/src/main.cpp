#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/compiler.h"
#include "../include/vm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <chrono>

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string generateRandomString(int length) {
    static const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(chars) - 2);
    
    std::string result;
    for (int i = 0; i < length; i++) {
        result += chars[dis(gen)];
    }
    return result;
}

void obfuscateFile(const std::string& filename, const std::string& source) {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    file << "# OBFUSCATED CODE - DO NOT EDIT\n";
    file << "# This file has been automatically obfuscated by ZobyScript\n";
    file << "# Original functionality preserved\n\n";
    
    file << "use obfuscator\n\n";
    
    std::map<std::string, std::string> varMap;
    std::vector<std::string> keywords = {"secret", "calculate", "result", "x", "y", "i"};
    
    for (const auto& var : keywords) {
        varMap[var] = "_" + generateRandomString(16);
    }
    
    std::string obfuscated = source;
    
    for (const auto& pair : varMap) {
        size_t pos = 0;
        while ((pos = obfuscated.find(pair.first, pos)) != std::string::npos) {
            bool isWord = (pos == 0 || !isalnum(obfuscated[pos-1])) &&
                         (pos + pair.first.length() >= obfuscated.length() || 
                          !isalnum(obfuscated[pos + pair.first.length()]));
            if (isWord) {
                obfuscated.replace(pos, pair.first.length(), pair.second);
                pos += pair.second.length();
            } else {
                pos++;
            }
        }
    }
    
    file << "# Obfuscation layer 1\n";
    for (int i = 0; i < 5; i++) {
        file << "_junk" << i << " = " << dis(gen) << "\n";
    }
    file << "\n";
    
    file << obfuscated;
    
    file << "\n\n# Obfuscation layer 2\n";
    for (int i = 0; i < 5; i++) {
        file << "_dead" << i << " = \"" << generateRandomString(20) << "\"\n";
    }
    
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <script.zs|script.zsc>" << std::endl;
        return 1;
    }
    
    try {
        std::string filename = argv[1];
        bool isBytecode = filename.size() >= 4 && filename.substr(filename.size() - 4) == ".zsc";
        
        Compiler compiler;
        Chunk mainChunk;
        std::string source;
        
        if (isBytecode) {
            std::cout << "[VM] Loading bytecode from '" << filename << "'..." << std::endl;
            mainChunk = compiler.loadBytecode(filename);
            if (mainChunk.code.empty()) {
                throw std::runtime_error("Failed to load bytecode file");
            }
            std::cout << "[VM] Bytecode loaded successfully!" << std::endl;
            std::cout << "[VM] Executing virtualized code..." << std::endl;
            std::cout << std::endl;
        } else {
            source = readFile(argv[1]);
            
            Lexer lexer(source);
            std::vector<Token> tokens = lexer.tokenize();
            
            Parser parser(tokens);
            auto program = parser.parse();
            
            mainChunk = compiler.compile(program.get());
        }
        
        if (compiler.isObfuscated()) {
            std::cout << "[OBFUSCATOR] Code obfuscated successfully!" << std::endl;
            std::cout << "[OBFUSCATOR] Bytecode encrypted with XOR cipher" << std::endl;
            std::cout << "[OBFUSCATOR] Variable names randomized" << std::endl;
            std::cout << "[OBFUSCATOR] Control flow flattened" << std::endl;
            std::cout << "[OBFUSCATOR] Running obfuscated code..." << std::endl;
            std::cout << std::endl;
        }
        
        VM vm;
        auto start = std::chrono::high_resolution_clock::now();
        vm.run(mainChunk, compiler.getFunctions());
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        if (!compiler.isObfuscated() && !isBytecode) {
            std::cout << "\n[Performance] Execution time: " << duration.count() << "ms" << std::endl;
        }
        
        if (compiler.isObfuscated() && !isBytecode) {
            std::cout << std::endl;
            std::cout << "[OBFUSCATOR] Obfuscating source file..." << std::endl;
            obfuscateFile(argv[1], source);
            std::cout << "[OBFUSCATOR] File '" << argv[1] << "' has been obfuscated!" << std::endl;
            
            std::string zscFile = std::string(argv[1]);
            size_t dotPos = zscFile.find_last_of(".");
            if (dotPos != std::string::npos) {
                zscFile = zscFile.substr(0, dotPos) + ".zsc";
            } else {
                zscFile += ".zsc";
            }
            
            compiler.saveBytecode(zscFile, mainChunk);
            std::cout << "[OBFUSCATOR] Bytecode saved to '" << zscFile << "'" << std::endl;
            std::cout << "[OBFUSCATOR] Run with: " << argv[0] << " " << zscFile << std::endl;
            std::cout << "[OBFUSCATOR] Original code replaced with obfuscated version." << std::endl;
            std::cout << "[OBFUSCATOR] Execution complete. Code remains obfuscated." << std::endl;
        } else if (isBytecode) {
            std::cout << std::endl;
            std::cout << "[VM] Execution complete. Bytecode remains protected." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
