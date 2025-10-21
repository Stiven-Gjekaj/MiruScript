/**
 * WASM-specific code generation
 *
 * This is a specialized version of codegen that writes to a buffer
 * instead of a FILE stream, making it suitable for WebAssembly.
 */

#include "codegen.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

/**
 * Buffer for collecting output
 */
typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} OutputBuffer;

static OutputBuffer g_wasm_buffer = {NULL, 0, 0};

/**
 * Append string to buffer
 */
static void buffer_append(const char *str) {
    if (!str) return;
    size_t len = strlen(str);
    if (len == 0) return;

    if (g_wasm_buffer.size + len >= g_wasm_buffer.capacity) {
        size_t new_cap = g_wasm_buffer.capacity * 2;
        if (new_cap == 0) new_cap = 8192;
        while (new_cap < g_wasm_buffer.size + len) new_cap *= 2;

        char *new_data = realloc(g_wasm_buffer.data, new_cap);
        if (!new_data) return;
        g_wasm_buffer.data = new_data;
        g_wasm_buffer.capacity = new_cap;
    }

    memcpy(g_wasm_buffer.data + g_wasm_buffer.size, str, len);
    g_wasm_buffer.size += len;
}

/**
 * Formatted append (like sprintf to buffer)
 */
static void buffer_appendf(const char *fmt, ...) {
    char temp[4096];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(temp, sizeof(temp), fmt, args);
    va_end(args);

    if (len > 0) {
        buffer_append(temp);
    }
}

/* Import from codegen.c - these are the actual implementations */
typedef struct CodeGen {
    FILE *output;
    int indent_level;
    bool in_function;
    ASTNode **functions;
    size_t function_count;
    size_t function_capacity;
} CodeGen;

/* Forward declarations - these are defined in codegen.c */
void emit_indent_to_buffer(int indent_level);
void emit_includes_to_buffer(void);
void emit_forward_declarations_to_buffer(CodeGen *gen);
void emit_expression_to_buffer(CodeGen *gen, ASTNode *node);
void emit_statement_to_buffer(CodeGen *gen, ASTNode *node);
void emit_statement_list_to_buffer(CodeGen *gen, ASTNode **statements, size_t count);
void emit_function_definition_to_buffer(CodeGen *gen, ASTNode *node);
void collect_functions(CodeGen *gen, ASTNode *ast);
bool has_top_level_statements(ASTNode *ast);

/**
 * Compile Miru source code and return generated C code
 *
 * This function:
 * 1. Lexes the source
 * 2. Parses into AST
 * 3. Generates C code to buffer
 * 4. Returns malloc'd string with the result
 *
 * Caller must free the returned string with miru_free_result()
 */
char* miru_compile(const char *source_code) {
    if (!source_code) {
        return NULL;
    }

    // Reset buffer
    g_wasm_buffer.size = 0;

    // Create lexer and parser
    Lexer *lexer = lexer_create(source_code);
    if (!lexer) {
        return NULL;
    }

    Parser *parser = parser_create(lexer);
    if (!parser) {
        lexer_destroy(lexer);
        return NULL;
    }

    // Parse source to AST
    ASTNode *ast = parser_parse(parser);

    parser_destroy(parser);
    lexer_destroy(lexer);

    if (!ast) {
        return NULL;
    }

    if (ast->type != NODE_PROGRAM) {
        ast_destroy(ast);
        return NULL;
    }

    // Create codegen (FILE* is not used)
    CodeGen gen = {
        .output = NULL,
        .indent_level = 0,
        .in_function = false,
        .functions = NULL,
        .function_count = 0,
        .function_capacity = 0
    };

    // Collect functions
    collect_functions(&gen, ast);

    // Emit includes
    buffer_append("#include <stdio.h>\n");
    buffer_append("#include <stdlib.h>\n");
    buffer_append("#include <string.h>\n");

    // Emit print function declarations
    buffer_append("\nvoid print_int(int value);\n");
    buffer_append("void print_float(double value);\n");
    buffer_append("void print_string(const char *value);\n");

    // Emit forward declarations for functions
    for (size_t i = 0; i < gen.function_count; i++) {
        ASTNode *func = gen.functions[i];
        if (func->type == NODE_FUNCTION_DEF) {
            const char *name = func->data.function_def.name;
            int param_count = func->data.function_def.param_count;

            buffer_append("int ");
            buffer_append(name);
            buffer_append("(");

            for (int j = 0; j < param_count; j++) {
                if (j > 0) buffer_append(", ");
                buffer_append("int ");
                buffer_append(func->data.function_def.params[j]);
            }

            buffer_append(");\n");
        }
    }

    // Emit print functions (runtime)
    buffer_append("\nvoid print_int(int value) {\n");
    buffer_append("    printf(\"%d\\n\", value);\n");
    buffer_append("}\n\n");

    buffer_append("void print_float(double value) {\n");
    buffer_append("    printf(\"%f\\n\", value);\n");
    buffer_append("}\n\n");

    buffer_append("void print_string(const char *value) {\n");
    buffer_append("    printf(\"%s\\n\", value);\n");
    buffer_append("}\n\n");

    // Emit function definitions - SIMPLIFIED (we'll improve this later)
    // For now, generate stub that works with buffer
    for (size_t i = 0; i < gen.function_count; i++) {
        ASTNode *func = gen.functions[i];
        if (func->type == NODE_FUNCTION_DEF) {
            const char *name = func->data.function_def.name;
            int param_count = func->data.function_def.param_count;

            buffer_append("int ");
            buffer_append(name);
            buffer_append("(");

            for (int j = 0; j < param_count; j++) {
                if (j > 0) buffer_append(", ");
                buffer_append("int ");
                buffer_append(func->data.function_def.params[j]);
            }

            buffer_append(") {\n");
            buffer_append("    return 0;  // TODO: implement function body\n");
            buffer_append("}\n\n");
        }
    }

    // Emit main function if there are top-level statements
    if (has_top_level_statements(ast)) {
        buffer_append("int main(void) {\n");
        buffer_append("    return 0;  // TODO: implement top-level statements\n");
        buffer_append("}\n");
    }

    // Cleanup
    free(gen.functions);
    ast_destroy(ast);

    // Copy buffer to malloc'd string
    char *result = malloc(g_wasm_buffer.size + 1);
    if (result) {
        memcpy(result, g_wasm_buffer.data, g_wasm_buffer.size);
        result[g_wasm_buffer.size] = '\0';
    }

    return result;
}

/**
 * Free memory allocated by miru_compile
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
