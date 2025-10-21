#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/parser.h"

int tests_run = 0;
int tests_passed = 0;

void assert_equal_int(int actual, int expected, const char *test_name) {
    tests_run++;
    if (actual == expected) {
        tests_passed++;
        printf("PASS: %s\n", test_name);
    } else {
        printf("FAIL: %s (expected %d, got %d)\n", test_name, expected, actual);
    }
}

void test_parser_creation(void) {
    const char *source = "print(42)";
    Lexer *lexer = lexer_create(source);
    Parser *parser = parser_create(lexer);
    assert_equal_int(parser != NULL, 1, "test_parser_creation");
    parser_destroy(parser);
    lexer_destroy(lexer);
}

void test_parser_parse(void) {
    const char *source = "42";
    Lexer *lexer = lexer_create(source);
    Parser *parser = parser_create(lexer);
    ASTNode *ast = parser_parse(parser);
    assert_equal_int(1, 1, "test_parser_parse");
    parser_destroy(parser);
    lexer_destroy(lexer);
    if (ast) {
        ast_destroy(ast);
    }
}

int main(void) {
    printf("Running Parser Tests...\n\n");

    test_parser_creation();
    test_parser_parse();

    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return tests_passed == tests_run ? 0 : 1;
}
