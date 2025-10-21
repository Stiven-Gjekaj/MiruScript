#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef enum {
    TOKEN_EOF,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_DOT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_ASSIGN,
    TOKEN_EQ,
    TOKEN_NE,
    TOKEN_LT,
    TOKEN_LE,
    TOKEN_GT,
    TOKEN_GE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_FUNC,
    TOKEN_RETURN,
    TOKEN_LET,
    TOKEN_CONST,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_PRINT,
    TOKEN_ERROR,
} TokenKind;

typedef struct {
    TokenKind kind;
    const char *lexeme;
    size_t length;
    int line;
    int column;
    union {
        long int_value;
        double float_value;
    } value;
} Token;

typedef struct {
    const char *source;
    size_t length;
    size_t pos;
    size_t line;
    size_t column;
} Lexer;

Lexer *lexer_create(const char *source);
void lexer_destroy(Lexer *lexer);
Token lexer_next_token(Lexer *lexer);

#endif
