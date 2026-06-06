#!/usr/bin/env bash
check_error()
{
    if [ $? -ne 0 ]; then
        echo "Build failed."
        read -p "Press Enter to continue..."
        exit 1
    fi
}

echo "Building Emscripten"

echo "Running CMake ..."

emcmake cmake -S . -B build
check_error

echo "Changing directory..."

cd build
echo "Changed directory, error level: $?"
check_error

echo "Running emmake..."
emmake make
check_error

echo "Build Successful!"
read -p "Press enter to continue";
exit 0