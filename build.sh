#!/bin/sh

# Сборка Boost.Log
echo "Building Boost.Log..."
cd libs/boost_1_78_0 || exit 1
./bootstrap.sh
./b2 install --with-log
cd ../..

# Сборка шлюза OKX
echo "Building OKX Gateway..."
mkdir -p build/Debug
cd build/Debug || exit 1
cmake ../..
cmake --build . --clean-first
