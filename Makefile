# Makefile for Miru Compiler
# A minimal C-based language compiler

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iruntime -Isrc
LDFLAGS =

# Directories
SRC_DIR = src
RUNTIME_DIR = runtime
TEST_DIR = tests
OUT_DIR = out

# Source files
COMPILER_SRCS = $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/ast.c $(SRC_DIR)/codegen.c $(SRC_DIR)/main.c
RUNTIME_SRCS = $(RUNTIME_DIR)/print.c

# Object files
COMPILER_OBJS = $(COMPILER_SRCS:.c=.o)
RUNTIME_OBJS = $(RUNTIME_SRCS:.c=.o)

# Targets
COMPILER_BIN = miru
RUNTIME_LIB = libmiru_runtime.a

# Default target
all: $(COMPILER_BIN) $(RUNTIME_LIB)

# Build the compiler
$(COMPILER_BIN): $(COMPILER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build the runtime library
$(RUNTIME_LIB): $(RUNTIME_OBJS)
	ar rcs $@ $^

# Compile C source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test targets
test: all
	cd $(TEST_DIR) && bash run_all.sh

# Clean build artifacts
clean:
	rm -f $(COMPILER_OBJS) $(RUNTIME_OBJS) $(COMPILER_BIN) $(RUNTIME_LIB)
	rm -f $(TEST_DIR)/*.o $(TEST_DIR)/test_lexer $(TEST_DIR)/test_parser
	rm -rf $(OUT_DIR)/*

# Create output directory
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Help target
help:
	@echo "Miru Compiler Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build compiler and runtime (default)"
	@echo "  test     - Run all tests"
	@echo "  clean    - Remove build artifacts"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CC       - C compiler (default: gcc)"
	@echo "  CFLAGS   - Compiler flags"

.PHONY: all test clean help
