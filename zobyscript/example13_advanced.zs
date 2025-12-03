# Advanced Features Demo

# Null values
x = null
print("x is null:", x == null)

# Ternary operator
age = 20
status = age >= 18 ? "adult" : "minor"
print("Status:", status)

# Hash maps
person = {"name": "Alice", "age": 25, "city": "NYC"}
print("Person:", person)
print("Name:", person["name"])
print("Age:", person["age"])

# String functions
text = "Hello World"
print("Upper:", upper(text))
print("Lower:", lower(text))

words = split("a,b,c", ",")
print("Split:", words)

joined = join(["x", "y", "z"], "-")
print("Joined:", joined)

# Hash map functions
keys_list = keys(person)
print("Keys:", keys_list)

values_list = values(person)
print("Values:", values_list)

# Break and continue
print("Break demo:")
for (i = 0; i < 10; i = i + 1) {
    if (i == 5) {
        break
    }
    print(i)
}

print("Continue demo:")
for (i = 0; i < 5; i = i + 1) {
    if (i == 2) {
        continue
    }
    print(i)
}
