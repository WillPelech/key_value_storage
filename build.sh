#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

echo "Build complete! Executable is at: build/kv"
