# Changelog

## Version 3.0

### New Features
- Added control flow statements (if/else, while, for)
- Implemented comparison operators (<, >, <=, >=, ==, !=)
- Added logical operators (and, or, not) with short-circuit evaluation
- Implemented arrays and hashmaps
- Added string data type with concatenation
- Implemented break and continue statements
- Added ternary operator (condition ? true_val : false_val)
- Implemented null value type

### Standard Library
- Math library (sqrt, pow, abs, floor, ceil, sin, cos, tan, random, min, max, round)
- String functions (upper, lower, split, join)
- Array functions (len, push, pop, keys, values)
- File I/O (read, write, append, exists, delete)
- Type conversion (str, num, type)
- User input (input)

### Performance Improvements
- Implemented constant folding optimization
- Added dead code elimination
- Optimized integer arithmetic with specialized opcodes
- Implemented string interning
- Changed global variable storage from map to vector for O(1) lookup
- Added inline caching for frequently accessed variables

### Other Changes
- Code obfuscator now modifies source files and saves bytecode
- Improved error messages
- Better memory management

## Version 2.0

### New Features
- User-defined functions with parameters and return values
- Local variable scoping
- Proper arithmetic operator precedence
- Bytecode compilation and execution
- Stack-based virtual machine

### Changes
- Rewrote parser to use precedence climbing
- Implemented AST-based compilation
- Added call frames for function execution

## Version 1.0

Initial release with basic features:
- Variables and assignments
- Basic arithmetic operations
- Print statement
- Simple expression evaluation
