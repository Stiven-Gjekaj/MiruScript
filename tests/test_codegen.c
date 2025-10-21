/*
 * Test for Code Generator
 * This file tests the code generation functionality
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/ast.h"
#include "../src/codegen.h"

/* Test helper to capture output to string */
static char *capture_codegen_output(ASTNode *ast) {
    char *output = NULL;
    size_t size = 0;

    /* Open memory stream (POSIX) or use tmpfile */
    FILE *stream = tmpfile();
    if (!stream) {
        return NULL;
    }

    /* Generate code */
    CodeGen *gen = codegen_create(stream);
    codegen_generate(gen, ast);
    codegen_destroy(gen);

    /* Read back the output */
    fseek(stream, 0, SEEK_END);
    size = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    output = malloc(size + 1);
    if (output) {
        fread(output, 1, size, stream);
        output[size] = '\0';
    }

    fclose(stream);
    return output;
}

/* Test 1: Simple integer literal */
void test_int_literal() {
    printf("Test 1: Integer literal... ");

    /* Create AST: program with single expression statement: 42 */
    ASTNode *program = ast_create_program();
    ASTNode *literal = ast_create_int_literal(42);
    ASTNode *expr_stmt = ast_create_expr_stmt(literal);
    ast_program_add_statement(program, expr_stmt);

    char *output = capture_codegen_output(program);

    /* Check that it contains the literal */
    assert(output != NULL);
    assert(strstr(output, "42") != NULL);
    assert(strstr(output, "#include") != NULL);
    assert(strstr(output, "int main") != NULL);

    free(output);
    ast_destroy(program);

    printf("PASSED\n");
}

/* Test 2: Function definition */
void test_function_def() {
    printf("Test 2: Function definition... ");

    /* Create AST: func add(a, b) { return a + b; } */
    ASTNode *program = ast_create_program();

    /* Create parameters */
    char **params = malloc(2 * sizeof(char *));
    params[0] = strdup("a");
    params[1] = strdup("b");

    /* Create function body: return a + b */
    ASTNode *a = ast_create_identifier("a");
    ASTNode *b = ast_create_identifier("b");
    ASTNode *add_expr = ast_create_binary_op(a, b, OP_ADD);
    ASTNode *return_stmt = ast_create_return(add_expr);

    ASTNode **body = malloc(sizeof(ASTNode *));
    body[0] = return_stmt;

    /* Create function */
    ASTNode *func = ast_create_function_def("add", params, 2, body, 1);
    ast_program_add_statement(program, func);

    char *output = capture_codegen_output(program);

    /* Verify output */
    assert(output != NULL);
    assert(strstr(output, "int add") != NULL);
    assert(strstr(output, "int a") != NULL);
    assert(strstr(output, "int b") != NULL);
    assert(strstr(output, "return") != NULL);

    free(output);
    ast_destroy(program);

    printf("PASSED\n");
}

/* Test 3: If statement */
void test_if_statement() {
    printf("Test 3: If statement... ");

    /* Create AST: if (1) { return 42; } */
    ASTNode *program = ast_create_program();

    /* Create condition and branches */
    ASTNode *cond = ast_create_int_literal(1);
    ASTNode *ret = ast_create_return(ast_create_int_literal(42));

    ASTNode **then_branch = malloc(sizeof(ASTNode *));
    then_branch[0] = ret;

    ASTNode *if_stmt = ast_create_if(cond, then_branch, 1, NULL, 0);
    ASTNode *expr_stmt = ast_create_expr_stmt(if_stmt);
    ast_program_add_statement(program, expr_stmt);

    char *output = capture_codegen_output(program);

    /* Verify output */
    assert(output != NULL);
    assert(strstr(output, "if") != NULL);
    assert(strstr(output, "return") != NULL);

    free(output);
    ast_destroy(program);

    printf("PASSED\n");
}

/* Test 4: Function call */
void test_function_call() {
    printf("Test 4: Function call... ");

    /* Create AST: print(42) */
    ASTNode *program = ast_create_program();

    ASTNode *func = ast_create_identifier("print");
    ASTNode **args = malloc(sizeof(ASTNode *));
    args[0] = ast_create_int_literal(42);

    ASTNode *call = ast_create_call(func, args, 1);
    ASTNode *expr_stmt = ast_create_expr_stmt(call);
    ast_program_add_statement(program, expr_stmt);

    char *output = capture_codegen_output(program);

    /* Verify output - should call miru_print_int */
    assert(output != NULL);
    assert(strstr(output, "miru_print_int") != NULL);
    assert(strstr(output, "42") != NULL);

    free(output);
    ast_destroy(program);

    printf("PASSED\n");
}

/* Test 5: Binary operations */
void test_binary_operations() {
    printf("Test 5: Binary operations... ");

    /* Create AST: 2 + 3 * 4 */
    ASTNode *program = ast_create_program();

    ASTNode *two = ast_create_int_literal(2);
    ASTNode *three = ast_create_int_literal(3);
    ASTNode *four = ast_create_int_literal(4);

    ASTNode *mul = ast_create_binary_op(three, four, OP_MUL);
    ASTNode *add = ast_create_binary_op(two, mul, OP_ADD);

    ASTNode *expr_stmt = ast_create_expr_stmt(add);
    ast_program_add_statement(program, expr_stmt);

    char *output = capture_codegen_output(program);

    /* Verify output */
    assert(output != NULL);
    assert(strstr(output, "2") != NULL);
    assert(strstr(output, "3") != NULL);
    assert(strstr(output, "4") != NULL);
    assert(strstr(output, "+") != NULL);
    assert(strstr(output, "*") != NULL);

    free(output);
    ast_destroy(program);

    printf("PASSED\n");
}

int main(void) {
    printf("\n=== Code Generator Tests ===\n\n");

    test_int_literal();
    test_function_def();
    test_if_statement();
    test_function_call();
    test_binary_operations();

    printf("\nAll tests passed!\n\n");

    return 0;
}
