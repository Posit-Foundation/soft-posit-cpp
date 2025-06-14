#!/bin/bash

# Find all C/C++ files and format them
find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -not -path "./build/*" -not -path "./install/*" | while read -r file; do
    echo "Formatting $file"
    clang-format -i "$file"
done

echo "Formatting complete!" 