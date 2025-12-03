# Optimized Performance Test

print("=== Optimized Performance Test ===")
print("")

# Test 1: 1 million integer additions
print("Test 1: 1,000,000 integer additions")
sum = 0
i = 0
while (i < 1000000) {
    sum = sum + i
    i = i + 1
}
print("Result:", sum)
print("")

# Test 2: Iterative fibonacci
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

# Test 3: Constant folding test
print("Test 3: Constant folding")
x = 2 + 3
y = 10 * 5
z = 100 - 25
print("x:", x, "y:", y, "z:", z)
print("")

# Test 4: Nested loops
print("Test 4: Nested loops (1000x100)")
count = 0
i = 0
while (i < 1000) {
    j = 0
    while (j < 100) {
        count = count + 1
        j = j + 1
    }
    i = i + 1
}
print("Count:", count)
print("")

print("=== Benchmark Complete ===")
