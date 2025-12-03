# Performance Benchmark with Timing

print("=== ZobyScript Performance Test ===")
print("")

# Test 1: Integer arithmetic
print("Test 1: 100,000 integer additions")
sum = 0
for (i = 0; i < 100000; i = i + 1) {
    sum = sum + i
}
print("Result:", sum)
print("")

# Test 2: Recursive fibonacci
print("Test 2: Recursive Fibonacci(25)")
func fib(n) {
    if (n <= 1) {
        return n
    }
    return fib(n - 1) + fib(n - 2)
}
result = fib(25)
print("Result:", result)
print("")

# Test 3: Array operations
print("Test 3: 10,000 array operations")
arr = []
for (i = 0; i < 10000; i = i + 1) {
    arr = push(arr, i)
}
print("Array length:", len(arr))
print("")

print("=== Benchmark Complete ===")
