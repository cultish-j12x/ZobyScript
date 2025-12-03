use math

func factorial(n) {
    if (n <= 1) {
        return 1
    }
    return n * factorial(n - 1)
}

func isPrime(n) {
    if (n < 2) {
        return false
    }
    i = 2
    while (i * i <= n) {
        if (n - floor(n / i) * i == 0) {
            return false
        }
        i = i + 1
    }
    return true
}

print("=== Factorial Calculator ===")
for (i = 1; i <= 10; i = i + 1) {
    print("factorial(", i, ") =", factorial(i))
}

print("")
print("=== Prime Numbers 1-20 ===")
primes = []
for (n = 1; n <= 20; n = n + 1) {
    if (isPrime(n)) {
        print(n, "is prime")
    }
}

print("")
print("=== Math Functions ===")
numbers = [4, 9, 16, 25, 36]
print("Numbers:", numbers)

for (i = 0; i < len(numbers); i = i + 1) {
    num = numbers[i]
    print("sqrt(", num, ") =", sqrt(num))
}

print("")
print("=== Random Numbers ===")
for (i = 0; i < 5; i = i + 1) {
    r = floor(random() * 100)
    print("Random:", r)
}
