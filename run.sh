#!/bin/bash

# Create the build directory if it doesn't exist
mkdir -p build

# Compile the C++ source file
g++ -o build/path-stretch path-stretch.cxx

# Check if compilation was successful
if [ $? -eq 0 ]; then
    # Run the executable
    ./build/path-stretch
else
    echo "Compilation failed."
    exit 1
fi
