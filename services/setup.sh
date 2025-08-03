#!/bin/bash

set -e

echo "🔧 Creating build directory..."
mkdir -p build

echo "🔨 Running cmake configuration..."
cmake -S . -B build

echo "🚀 Building project..."
cmake --build build -j$(nproc)

echo "✅ Build complete!"
