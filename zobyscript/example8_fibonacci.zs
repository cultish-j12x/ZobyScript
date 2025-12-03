func fib(n) {
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

for (i = 0 i < 10 i = i + 1) {
    result = fib(i)
    print("fib(", i, ") =", result)
}
