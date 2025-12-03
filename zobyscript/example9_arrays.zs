use math

numbers = [1, 2, 3, 4, 5]
print("Array:", numbers)

print("First element:", numbers[0])
print("Third element:", numbers[2])

print("Array length:", len(numbers))

names = ["Alice", "Bob", "Charlie"]
print("Names:", names)

for (i = 0; i < len(names); i = i + 1) {
    print("Hello,", names[i])
}

mixed = [42, "text", true, [1, 2, 3]]
print("Mixed array:", mixed)
print("Nested array:", mixed[3])
