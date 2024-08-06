#!/bin/bash

# Check if the test script path was provided
if [ $# -lt 1 ]; then
    echo "Usage: $0 <test_script_path>"
    exit 1
fi

TEST_SCRIPT_PATH=$1

# Compile and link the main application
clang++ -Wall src/main.cpp -o src/build/main $(llvm-config --cxxflags --ldflags --system-libs --libs core irreader support)

# Run the test script
./src/build/main "$TEST_SCRIPT_PATH"

# Generate object file from LLVM bitcode
llc -filetype=obj ./src/build/out.ll -o ./src/build/out.o

# Link the final executable
clang ./src/build/out.o -o ./src/build/out

# Execute the final binary
./src/build/out
