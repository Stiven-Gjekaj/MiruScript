/**
 * Miru Compiler - WASM Wrapper
 *
 * This file provides a simple API for compiling Miru source code to C.
 * Designed to be compiled to WebAssembly for browser-based IDE.
 *
 * Usage:
 *   const result = miru_compile(source_code);
 *   if (result) printf("%s\n", result);
 *   miru_free_result(result);
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

/**
 * Internal buffer for storing compiler output
 */
typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} StringBuffer;

/**
 * Custom FILE-like structure for capturing output
 */
static StringBuffer g_output = {0};

/**
 * Append to output buffer
 */
static void buffer_append(const char *str, size_t len) {
    if (!str || len == 0) return;

    if (g_output.size + len >= g_output.capacity) {
        size_t new_capacity = g_output.capacity * 2;
        if (new_capacity == 0) new_capacity = 4096;
        while (new_capacity < g_output.size + len) {
            new_capacity *= 2;
        }

        char *new_data = realloc(g_output.data, new_capacity);
        if (!new_data) return;

        g_output.data = new_data;
        g_output.capacity = new_capacity;
    }

    memcpy(g_output.data + g_output.size, str, len);
    g_output.size += len;
}

/**
 * Buffer printf - writes formatted string to buffer
 */
static int buffer_printf(const char *fmt, ...) {
    char temp[8192];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(temp, sizeof(temp), fmt, args);
    va_end(args);

    if (len > 0) {
        buffer_append(temp, len);
    }
    return len;
}

/**
 * Buffer putchar
 */
static int buffer_putchar(int c) {
    char ch = (char)c;
    buffer_append(&ch, 1);
    return c;
}

/**
 * Replace CodeGen's FILE output with our buffer
 * This is a bit hacky but works for the MVP
 */
typedef struct CodeGen {
    // We'll write directly to the buffer instead of FILE*
    int indent_level;
    bool in_function;
    ASTNode **functions;
    size_t function_count;
} CodeGen;

/**
 * Modified codegen_generate that writes to buffer
 */
extern void codegen_generate_to_buffer(CodeGen *gen, ASTNode *ast);

/**
 * Main compilation function
 *
 * Input: source_code - Miru source code as null-terminated string
 * Output: Generated C code, or error message
 * Memory: Allocated with malloc(), must be freed with miru_free_result()
 */
char* miru_compile(const char *source_code) {
    if (!source_code) {
        const char *error = "Error: No source code provided\n";
        char *result = malloc(strlen(error) + 1);
        if (result) strcpy(result, error);
        return result;
    }

    // Reset output buffer
    g_output.size = 0;

    // Create lexer
    Lexer *lexer = lexer_create(source_code);
    if (!lexer) {
        const char *error = "Error: Failed to create lexer\n";
        char *result = malloc(strlen(error) + 1);
        if (result) strcpy(result, error);
        return result;
    }

    // Create parser
    Parser *parser = parser_create(lexer);
    if (!parser) {
        lexer_destroy(lexer);
        const char *error = "Error: Failed to create parser\n";
        char *result = malloc(strlen(error) + 1);
        if (result) strcpy(result, error);
        return result;
    }

    // Parse
    ASTNode *ast = parser_parse(parser);
    if (!ast) {
        parser_destroy(parser);
        lexer_destroy(lexer);
        const char *error = "Error: Failed to parse source code\n";
        char *result = malloc(strlen(error) + 1);
        if (result) strcpy(result, error);
        return result;
    }

    // Create code generator (FILE* is ignored, we write to buffer)
    CodeGen *codegen = codegen_create(stdout);
    if (!codegen) {
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        const char *error = "Error: Failed to create code generator\n";
        char *result = malloc(strlen(error) + 1);
        if (result) strcpy(result, error);
        return result;
    }

    // Generate code to buffer
    codegen_generate_to_buffer(codegen, ast);

    // Cleanup
    codegen_destroy(codegen);
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);

    // Null-terminate and copy result
    if (g_output.size == 0) {
        const char *error = "Error: No output generated\n";
        char *result = malloc(strlen(error) + 1);
        if (result) strcpy(result, error);
        return result;
    }

    char *result = malloc(g_output.size + 1);
    if (result) {
        memcpy(result, g_output.data, g_output.size);
        result[g_output.size] = '\0';
    }

    return result;
}

/**
 * Free result allocated by miru_compile()
 */
void miru_free_result(char *ptr) {
    free(ptr);
}

/**
 * Get compiler version
 */
const char* miru_version(void) {
    return "1.0.0";
}

/**
 * Cleanup internal buffer (call when exiting WASM)
 */
void miru_cleanup(void) {
    if (g_output.data) {
        free(g_output.data);
        g_output.data = NULL;
        g_output.size = 0;
        g_output.capacity = 0;
    }
}
