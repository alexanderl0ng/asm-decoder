#!/bin/bash

COMPILER="clang++"

BUILD_MODE="debug"

for arg in "$@"; do
    case $arg in
        g++|clang++)
            COMPILER="$arg"
            ;;
        debug|release)
            BUILD_MODE="$arg"
            ;;
        *)
            echo "Unknown Argument: $arg"
            echo "Usage: $0 [g++|clang++] [debug|release]"
            exit 1
            ;;
    esac
done

if [ "$BUILD_MODE" = "debug" ]; then
    FLAGS="-g -O0 -DDEBUG"
else
    FLAGS="-O2 -DNDEBUG"
fi

mkdir -p build

$COMPILER $FLAGS -o build/sim8086 src/sim8086.cpp

if [ $? -eq 0 ]; then
    echo "[SUCCESS] Compilation successful! Executable created in: build/"
else
    echo "[ERROR] Compilation failed"
    exit 1
fi

