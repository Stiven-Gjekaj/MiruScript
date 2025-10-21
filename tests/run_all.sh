#!/bin/bash

BUILD_DIR="build"
mkdir -p "$BUILD_DIR"

echo "Compiling tests..."
gcc -I.. -o "$BUILD_DIR/test_lexer" test_lexer.c ../src/lexer.c
gcc -I.. -o "$BUILD_DIR/test_parser" test_parser.c ../src/lexer.c ../src/parser.c ../src/ast.c

echo ""
echo "Running Lexer Tests..."
"$BUILD_DIR/test_lexer"
lexer_result=$?

echo ""
echo "Running Parser Tests..."
"$BUILD_DIR/test_parser"
parser_result=$?

echo ""
if [ $lexer_result -eq 0 ] && [ $parser_result -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed!"
    exit 1
fi
