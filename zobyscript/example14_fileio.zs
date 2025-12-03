# File I/O Demo

# Write to file
success = write("test.txt", "Hello from ZobyScript!")
print("Write success:", success)

# Read from file
content = read("test.txt")
print("File content:", content)

# Append to file
append("test.txt", "\nNew line added!")

# Read again
content2 = read("test.txt")
print("Updated content:", content2)

# Check if file exists
file_exists = exists("test.txt")
print("File exists:", file_exists)

# Create log file
write("log.txt", "Log started\n")
append("log.txt", "Event 1\n")
append("log.txt", "Event 2\n")
append("log.txt", "Event 3\n")

log_content = read("log.txt")
print("Log file:")
print(log_content)

# Delete file
deleted = delete("test.txt")
print("File deleted:", deleted)

# Check again
still_exists = exists("test.txt")
print("Still exists:", still_exists)
