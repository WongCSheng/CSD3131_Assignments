#!/usr/bin/env bash
# Build all questions into ./bin
# Run from the project root: ./build.sh
set -e
cd "$(dirname "$0")"

mkdir -p bin out
CXX="g++ -O0 -std=c++17" # -O2 for optimisation??

$CXX A2_Q1/q1.cpp lib/energy.cpp -o bin/q1
$CXX Q4/q4.cpp    lib/energy.cpp -o bin/q4
# $CXX Q3/q3.cpp  lib/energy.cpp -o bin/q3

echo "Build OK"
