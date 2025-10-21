#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", argv[1]);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *source = malloc(file_size + 1);
    if (!source) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return 1;
    }
    size_t bytes_read = fread(source, 1, file_size, file);
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Error: Failed to read file\n");
        free(source);
        fclose(file);
        return 1;
    }
    source[file_size] = '\0';
    fclose(file);

    Lexer *lexer = lexer_create(source);
    Parser *parser = parser_create(lexer);

    ASTNode *ast = parser_parse(parser);

    CodeGen *codegen = codegen_create(stdout);
    codegen_generate(codegen, ast);

    codegen_destroy(codegen);
    parser_destroy(parser);
    lexer_destroy(lexer);
    free(source);

    if (ast) {
        ast_destroy(ast);
    }

    return 0;
}
