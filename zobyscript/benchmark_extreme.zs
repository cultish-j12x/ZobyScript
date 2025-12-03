# Extreme Performance Benchmark

print("=== EXTREME Performance Benchmark ===")
print("")

# Test 1: 10 million integer additions
print("Test 1: 10,000,000 integer additions")
sum = 0
i = 0
while (i < 10000000) {
    sum = sum + i
    i = i + 1
}
print("Result:", sum)
print("")

# Test 2: Tight loop multiplication
print("Test 2: 1,000,000 multiplications")
result = 1
i = 0
while (i < 1000000) {
    result = result * 2
    result = result / 2
    i = i + 1
}
print("Result:", result)
print("")

# Test 3: Nested loops (1M iterations)
print("Test 3: Nested loops (10000x100)")
count = 0
i = 0
while (i < 10000) {
    j = 0
    while (j < 100) {
        count = count + 1
        j = j + 1
    }
    i = i + 1
}
print("Count:", count)
print("")

# Test 4: Comparison heavy
print("Test 4: 1,000,000 comparisons")
count = 0
i = 0
while (i < 1000000) {
    if (i < 500000) {
        count = count + 1
    }
    i = i + 1
}
print("Count:", count)
print("")

print("=== EXTREME Benchmark Complete ===")
