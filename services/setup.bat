@echo off
echo Creating build directory...
mkdir build 2>nul

echo Running CMake configuration...
cmake -S . -B build

echo Building project...
cmake --build build --parallel

echo Build complete.
