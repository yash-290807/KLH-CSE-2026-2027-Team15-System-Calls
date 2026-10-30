#!/bin/bash

echo "===== EDGE CASE TESTS ====="

name="Bharath Kumar"

# Spaces
echo "Name: $name"

# Literal variable
echo 'Name: $name'

# Expanded variable
echo "Name: $name"

# Empty string
empty=""
echo "Empty string length: ${#empty}"

# Special characters
special='! @ # $ % ^ & *'
echo "Special: $special"

# Nested quotes
echo "He said 'Hello World'"

# Command substitution
echo "Current directory: $(pwd)"

# Quoted filename
file="file with spaces.txt"

touch "$file"

if [ -f "$file" ]; then
    echo "PASS: File with spaces created."
fi

rm "$file"

echo "===== TEST COMPLETE ====="
