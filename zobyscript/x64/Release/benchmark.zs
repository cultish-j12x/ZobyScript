# Benchmark Script - Test Optimizations

print("=== ZobyScript Performance Benchmark ===")
print("")

# Test 1: Integer arithmetic (benefits from integer fast path)
print("Test 1: Integer Arithmetic")
sum = 0
for (i = 0; i < 10000; i = i + 1) {
    sum = sum + i
}
print("Sum:", sum)

# Test 2: Constant folding (2 + 3 should be folded to 5)
print("Test 2: Constant Folding")
x = 2 + 3 * 4
y = (10 + 5) * 2
z = 100 - 50 + 25
print("x:", x, "y:", y, "z:", z)

# Test 3: Dead code elimination
print("Test 3: Dead Code (should be eliminated)")
if (false) {
    print("This should never print!")
    x = 999999
}
if (true) {
    print("This always prints")
}

# Test 4: Inline caching (repeated global access)
print("Test 4: Inline Caching")
counter = 0
for (i = 0; i < 1000; i = i + 1) {
    counter = counter + 1
}
print("Counter:", counter)

# Test 5: Fibonacci (tests function calls and recursion)
print("Test 5: Fibonacci")
func fib(n) {
    if (n <= 1) {
        return n
    }
    return fib(n - 1) + fib(n - 2)
}

result = fib(20)
print("fib(20):", result)

print("")
print("=== Benchmark Complete ===")
