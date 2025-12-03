# Fast Benchmark - No Expensive Recursion

print("=== Fast Performance Test ===")
print("")

# Test 1: 1 million integer additions
print("Test 1: 1,000,000 integer additions")
sum = 0
for (i = 0; i < 1000000; i = i + 1) {
    sum = sum + i
}
print("Result:", sum)
print("")

# Test 2: Iterative fibonacci (fast)
print("Test 2: Iterative Fibonacci(1000)")
func fib_iter(n) {
    if (n <= 1) {
        return n
    }
    a = 0
    b = 1
    i = 2
    while (i <= n) {
        temp = a + b
        a = b
        b = temp
        i = i + 1
    }
    return b
}
result = fib_iter(1000)
print("Result:", result)
print("")

# Test 3: Nested loops
print("Test 3: Nested loops (1000x100)")
count = 0
for (i = 0; i < 1000; i = i + 1) {
    for (j = 0; j < 100; j = j + 1) {
        count = count + 1
    }
}
print("Count:", count)
print("")

print("=== Benchmark Complete ===")
