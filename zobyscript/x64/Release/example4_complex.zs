func fibonacci(n) {
    a = 0
    b = 1
    return a + b
}

func square(x) {
    return x * x
}

func area(width, height) {
    return width * height
}

x = 5
y = square(x)
print(y)

rect = area(10, 20)
print(rect)

result = square(3) + square(4)
print(result)

nested = area(square(2), square(3))
print(nested)
