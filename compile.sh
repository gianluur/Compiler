#!/bin/bash

# Check if the user provided an input argument
if [ -z "$1" ]; then
    echo "Usage: $0 <input>"
    exit 1
fi

# Store the input argument in a variable
user_input=$1

# Compile the main.cpp file
clang++ -Wall src/main.cpp -o build/main $(llvm-config --cxxflags --ldflags --system-libs --libs core irreader support)

# Run the compiled program with user input
./build/main "$user_input"

# Generate the object file from LLVM intermediate representation
llc -filetype=obj ./build/out.ll -o ./build/out.o

# Link the object file to create the final executable
clang ./build/out.o -o ./build/out

# Run the final executable
./build/out
