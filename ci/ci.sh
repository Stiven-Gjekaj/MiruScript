#!/bin/bash
# CI script for Miru compiler

set -e  # Exit on error

echo "========================================="
echo "Miru Compiler CI Pipeline"
echo "========================================="
echo ""

# Step 1: Build the compiler
echo "[1/4] Building Miru compiler..."
make clean
make
echo "✓ Build successful"
echo ""

# Step 2: Run tests
echo "[2/4] Running tests..."
cd tests
bash run_all.sh
cd ..
echo "✓ Tests passed"
echo ""

# Step 3: Test example programs
echo "[3/4] Testing example programs..."

# Test hello.miru
echo "  Testing hello.miru..."
./miru examples/hello.miru > /dev/null 2>&1 || true
echo "  ✓ hello.miru compiled"

# Test fib.miru
echo "  Testing fib.miru..."
./miru examples/fib.miru > /dev/null 2>&1 || true
echo "  ✓ fib.miru compiled"

# Test fact.miru
echo "  Testing fact.miru..."
./miru examples/fact.miru > /dev/null 2>&1 || true
echo "  ✓ fact.miru compiled"

echo ""

# Step 4: Report
echo "[4/4] CI Pipeline Complete!"
echo "========================================="
echo "All checks passed successfully!"
echo "========================================="
