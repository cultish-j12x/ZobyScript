# ZobyScript
ZobyScript is a lightweight, experimental programming language created entirely from scratch using C++.

<div align="center">

# 🌌 **ZobyScript Programming Language**
### *A Modern, Fast, Obfuscation-Powered Scripting Language*

**Created by a 16-year-old developer learning C++, Assembly & Compiler Engineering.**  
Built with ❤️ by **Cult** (GitHub: `CultWasTaken`)

<br>

![Stars](https://img.shields.io/github/stars/CultWasTaken/ZobyScript?color=8A2BE2&style=for-the-badge)
![Issues](https://img.shields.io/github/issues/CultWasTaken/ZobyScript?color=red&style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-orange?style=for-the-badge)
![Made With](https://img.shields.io/badge/Made_With-C++-blue?style=for-the-badge)

</div>

---

## ✨ **What Is ZobyScript?**

ZobyScript is a **fully custom programming language** written in **modern C++**, featuring:

- Custom **Lexer**
- Custom **Parser**
- Custom **Compiler**
- Custom **Bytecode Virtual Machine**
- `.zsc` **protected bytecode format**
- Built-in **Code Obfuscator + VM Virtualization**
- Clean & simple syntax  
- File I/O, Arrays, Hash Maps, Functions  
- Ternary, Null, break/continue  
- High optimization potential

ZobyScript is designed to be:

⚡ Fast  
🔒 Secure  
🧠 Learnable  
🔥 Fun as hell


---

## 🧒 **Who Made This?**

> **👤 Cult — 16-year-old self-taught developer**  
> Learning **C++, Assembly, compiler engineering**  
> Knows **Python, JavaScript**, reverse engineering

---

## 🔥 **Example Code (Simple)**

```zs
a = 100
b = 5
print(a + b)
🧠 Example: Functions + Loops
zs
Copy code
func fib(n) {
    if (n <= 1) return n
    a = 0
    b = 1
    for (i = 2; i < n; i = i + 1) {
        t = a + b
        a = b
        b = t
    }
    return b
}

print(fib(10))
🧰 Features
✔ Language Features
Variables

Functions

Arrays

Hash maps

Strings + manipulation

Ternary operator

Null

Loops

Break & Continue

Comparisons

Arithmetic

File I/O

✔ VM Features
Stack-based VM

Bytecode format (.zsc)

Instruction dispatch loop

Value system

Memory management

✔ Obfuscation Features
Variable renaming

Control-flow flattening

Junk instructions

Bytecode encryption

Virtualization layer

Anti-debug patterns

📁 File I/O Example
zs
Copy code
write("log.txt", "Hello!")
append("log.txt", "More text")
data = read("log.txt")
print(data)
🚀 Performance Benchmarks
Extreme Benchmark
diff
Copy code
=== EXTREME Performance Benchmark ===

10,000,000 additions
1,000,000 multiplications
10,000×100 nested loops
1,000,000 comparisons

Total execution time: ~5 seconds
Optimized Benchmark
mathematica
Copy code
Constant folding
Iterative Fibonacci(1000)
Array operations
String interning

Execution time: 411ms
🛠 Build Instructions
bash
Copy code
git clone https://github.com/CultWasTaken/ZobyScript
cd ZobyScript
cmake .
cmake --build .
Run scripts:

arduino
Copy code
zobyscript.exe file.zs
zobyscript.exe file.zsc  # runs virtualized bytecode
🧩 Directory Layout
css
Copy code
ZobyScript/
│
├── src/
│   ├── lexer.cpp
│   ├── parser.cpp
│   ├── compiler.cpp
│   ├── vm.cpp
│   ├── obfuscator.cpp
│   └── main.cpp
│
├── examples/
├── benchmarks/
├── README.md
└── LICENSE
🛣 Roadmap
🔥 Short Term
Better hash map performance

Register-based VM

Error messages with line numbers

REPL mode

⚡ Medium Term
Class system

Native module system

Optimizing compiler

🚀 Long Term
JIT Compiler

LLVM Backend

Multi-threading

Hot-path tracing

📜 License
ZobyScript is available under the MIT License.

⭐ Support the Project
If you like ZobyScript, consider starring the repo:

⭐ → It motivates development
⭐ → Helps visibility
⭐ → Shows support for a 16-year-old building a real programming language

<div align="center">
Made with ❤️ by Cult
“Learning C++, Assembly, reverse engine
