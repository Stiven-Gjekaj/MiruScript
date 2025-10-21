#include <stdio.h>
#include "src/lexer.h"

static const char* token_kind_to_string(TokenKind kind) {
    switch (kind) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_INT: return "INT";
        case TOKEN_FLOAT: return "FLOAT";
        case TOKEN_STRING: return "STRING";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COLON: return "COLON";
        case TOKEN_DOT: return "DOT";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_PERCENT: return "PERCENT";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_EQ: return "EQ";
        case TOKEN_NE: return "NE";
        case TOKEN_LT: return "LT";
        case TOKEN_LE: return "LE";
        case TOKEN_GT: return "GT";
        case TOKEN_GE: return "GE";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FOR: return "FOR";
        case TOKEN_FUNC: return "FUNC";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_LET: return "LET";
        case TOKEN_CONST: return "CONST";
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_NULL: return "NULL";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void print_token(Token token) {
    printf("[%d:%d] %s", token.line, token.column, token_kind_to_string(token.kind));

    if (token.kind == TOKEN_INT) {
        printf(" (%ld)", token.value.int_value);
    } else if (token.kind == TOKEN_FLOAT) {
        printf(" (%f)", token.value.float_value);
    } else if (token.length > 0 && token.kind != TOKEN_ERROR) {
        printf(" '%.*s'", (int)token.length, token.lexeme);
    } else if (token.kind == TOKEN_ERROR) {
        printf(" %s", token.lexeme);
    }

    printf("\n");
}

void test_lexer(const char* source) {
    printf("\n=== Testing Source ===\n%s\n", source);
    printf("=== Tokens ===\n");

    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        printf("Failed to create lexer\n");
        return;
    }

    Token token;
    do {
        token = lexer_next_token(lexer);
        print_token(token);
    } while (token.kind != TOKEN_EOF);

    lexer_destroy(lexer);
}

int main(void) {
    /* Test case from requirements */
    test_lexer("var x = 42;");

    /* Test keywords */
    test_lexer("if else while for func return let const true false null print");

    /* Test operators */
    test_lexer("+ - * / % = == != < <= > >= && || !");

    /* Test numbers */
    test_lexer("42 -100 3.14 -2.5 0.5");

    /* Test strings */
    test_lexer("\"hello\" \"world\" \"test string\"");

    /* Test delimiters */
    test_lexer("( ) { } [ ] , ; : .");

    /* Test comments */
    test_lexer("// this is a comment\nx = 5; // another comment");

    /* Complex example */
    test_lexer(
        "func factorial(n) {\n"
        "    if (n <= 1) {\n"
        "        return 1;\n"
        "    }\n"
        "    return n * factorial(n - 1);\n"
        "}\n"
    );

    /* Test error cases */
    test_lexer("& invalid");
    test_lexer("| invalid");
    test_lexer("\"unterminated string");

    return 0;
}
