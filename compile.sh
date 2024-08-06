#!/bin/bash

# Check if the user provided an input argument
if [ -z "$1" ]; then
    echo "Usage: $0 <input>"
    exit 1
fi

# Store the input argument in a variable
user_input=$1

# Compile the main.cpp file
clang++ -Wall src/main.cpp -o src/build/main $(llvm-config --cxxflags --ldflags --system-libs --libs core irreader support)

# Run the compiled program with user input
./src/build/main "$user_input"

# Generate the object file from LLVM intermediate representation
llc -filetype=obj ./src/build/out.ll -o ./src/build/out.o

# Link the object file to create the final executable
clang ./src/build/out.o -o ./src/build/out

# Run the final executable
./src/build/out
