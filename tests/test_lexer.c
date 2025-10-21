#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/lexer.h"

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

void test_lexer_creation(void) {
    const char *source = "print(42)";
    Lexer *lexer = lexer_create(source);
    assert_equal_int(lexer != NULL, 1, "test_lexer_creation");
    lexer_destroy(lexer);
}

void test_lexer_token_creation(void) {
    const char *source = "42";
    Lexer *lexer = lexer_create(source);
    Token token = lexer_next_token(lexer);
    assert_equal_int(token.kind != TOKEN_ERROR, 1, "test_lexer_token_creation");
    lexer_destroy(lexer);
}

int main(void) {
    printf("Running Lexer Tests...\n\n");

    test_lexer_creation();
    test_lexer_token_creation();

    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return tests_passed == tests_run ? 0 : 1;
}
