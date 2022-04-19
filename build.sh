#!/bin/sh

# Сборка шлюза OKX
echo "Building OKX Gateway..."
mkdir -p build/Debug
cd build/Debug || exit 1
cmake ../..
cmake --build .
