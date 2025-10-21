#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* Helper function prototypes */
static void skip_whitespace(Lexer *lexer);
static void skip_comment(Lexer *lexer);
static bool is_alpha(char c);
static bool is_digit(char c);
static bool is_alnum(char c);
static char peek(Lexer *lexer);
static char peek_next(Lexer *lexer);
static char advance(Lexer *lexer);
static bool match(Lexer *lexer, char expected);
static Token make_token(Lexer *lexer, TokenKind kind, const char *start, size_t length, int line, int column);
static Token error_token(Lexer *lexer, const char *message, int line, int column);
static Token parse_number(Lexer *lexer, int start_line, int start_column);
static Token parse_string(Lexer *lexer, int start_line, int start_column);
static Token parse_identifier(Lexer *lexer, int start_line, int start_column);
static TokenKind keyword_or_identifier(const char *text, size_t length);

Lexer *lexer_create(const char *source) {
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer) {
        return NULL;
    }
    lexer->source = source;
    lexer->length = source ? strlen(source) : 0;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->column = 1;
    return lexer;
}

void lexer_destroy(Lexer *lexer) {
    if (lexer) {
        free(lexer);
    }
}

/* Skip whitespace characters */
static void skip_whitespace(Lexer *lexer) {
    while (lexer->pos < lexer->length) {
        char c = lexer->source[lexer->pos];
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
                lexer->pos++;
                lexer->column++;
                break;
            case '\n':
                lexer->pos++;
                lexer->line++;
                lexer->column = 1;
                break;
            default:
                return;
        }
    }
}

/* Skip single-line comments starting with // */
static void skip_comment(Lexer *lexer) {
    if (peek(lexer) == '/' && peek_next(lexer) == '/') {
        /* Skip until end of line or end of file */
        while (lexer->pos < lexer->length && lexer->source[lexer->pos] != '\n') {
            lexer->pos++;
        }
    }
}

/* Check if character is alphabetic or underscore */
static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/* Check if character is a digit */
static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

/* Check if character is alphanumeric or underscore */
static bool is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

/* Peek at current character without advancing */
static char peek(Lexer *lexer) {
    if (lexer->pos >= lexer->length) {
        return '\0';
    }
    return lexer->source[lexer->pos];
}

/* Peek at next character without advancing */
static char peek_next(Lexer *lexer) {
    if (lexer->pos + 1 >= lexer->length) {
        return '\0';
    }
    return lexer->source[lexer->pos + 1];
}

/* Advance to next character and return current */
static char advance(Lexer *lexer) {
    if (lexer->pos >= lexer->length) {
        return '\0';
    }
    char c = lexer->source[lexer->pos];
    lexer->pos++;
    lexer->column++;
    return c;
}

/* Conditionally advance if next character matches expected */
static bool match(Lexer *lexer, char expected) {
    if (lexer->pos >= lexer->length) {
        return false;
    }
    if (lexer->source[lexer->pos] != expected) {
        return false;
    }
    lexer->pos++;
    lexer->column++;
    return true;
}

/* Create a token */
static Token make_token(Lexer *lexer, TokenKind kind, const char *start, size_t length, int line, int column) {
    Token token;
    token.kind = kind;
    token.lexeme = start;
    token.length = length;
    token.line = line;
    token.column = column;
    token.value.int_value = 0;
    return token;
}

/* Create an error token */
static Token error_token(Lexer *lexer, const char *message, int line, int column) {
    Token token;
    token.kind = TOKEN_ERROR;
    token.lexeme = message;
    token.length = strlen(message);
    token.line = line;
    token.column = column;
    token.value.int_value = 0;
    return token;
}

/* Parse a number (integer or float) */
static Token parse_number(Lexer *lexer, int start_line, int start_column) {
    const char *start = &lexer->source[lexer->pos - 1];
    bool is_float = false;

    /* Handle negative numbers - already advanced past the '-' */
    /* Continue parsing digits */
    while (is_digit(peek(lexer))) {
        advance(lexer);
    }

    /* Check for decimal point */
    if (peek(lexer) == '.' && is_digit(peek_next(lexer))) {
        is_float = true;
        advance(lexer); /* Consume '.' */

        while (is_digit(peek(lexer))) {
            advance(lexer);
        }
    }

    size_t length = &lexer->source[lexer->pos] - start;
    Token token = make_token(lexer, is_float ? TOKEN_FLOAT : TOKEN_INT, start, length, start_line, start_column);

    /* Parse the value */
    if (is_float) {
        token.value.float_value = strtod(start, NULL);
    } else {
        token.value.int_value = strtol(start, NULL, 10);
    }

    return token;
}

/* Parse a string literal */
static Token parse_string(Lexer *lexer, int start_line, int start_column) {
    const char *start = &lexer->source[lexer->pos - 1]; /* Include opening quote */

    while (peek(lexer) != '"' && peek(lexer) != '\0') {
        if (peek(lexer) == '\n') {
            lexer->line++;
            lexer->column = 0; /* Will be incremented by advance */
        }
        advance(lexer);
    }

    if (peek(lexer) == '\0') {
        return error_token(lexer, "Unterminated string", start_line, start_column);
    }

    advance(lexer); /* Consume closing quote */

    size_t length = &lexer->source[lexer->pos] - start;
    return make_token(lexer, TOKEN_STRING, start, length, start_line, start_column);
}

/* Parse an identifier or keyword */
static Token parse_identifier(Lexer *lexer, int start_line, int start_column) {
    const char *start = &lexer->source[lexer->pos - 1];

    while (is_alnum(peek(lexer))) {
        advance(lexer);
    }

    size_t length = &lexer->source[lexer->pos] - start;
    TokenKind kind = keyword_or_identifier(start, length);

    return make_token(lexer, kind, start, length, start_line, start_column);
}

/* Check if identifier is a keyword */
static TokenKind keyword_or_identifier(const char *text, size_t length) {
    /* Check keywords by length first for efficiency */
    switch (length) {
        case 2:
            if (strncmp(text, "if", 2) == 0) return TOKEN_IF;
            break;
        case 3:
            if (strncmp(text, "for", 3) == 0) return TOKEN_FOR;
            if (strncmp(text, "let", 3) == 0) return TOKEN_LET;
            break;
        case 4:
            if (strncmp(text, "else", 4) == 0) return TOKEN_ELSE;
            if (strncmp(text, "func", 4) == 0) return TOKEN_FUNC;
            if (strncmp(text, "true", 4) == 0) return TOKEN_TRUE;
            if (strncmp(text, "null", 4) == 0) return TOKEN_NULL;
            break;
        case 5:
            if (strncmp(text, "while", 5) == 0) return TOKEN_WHILE;
            if (strncmp(text, "false", 5) == 0) return TOKEN_FALSE;
            if (strncmp(text, "const", 5) == 0) return TOKEN_CONST;
            if (strncmp(text, "print", 5) == 0) return TOKEN_PRINT;
            break;
        case 6:
            if (strncmp(text, "return", 6) == 0) return TOKEN_RETURN;
            break;
    }

    return TOKEN_IDENTIFIER;
}

/* Main tokenization function */
Token lexer_next_token(Lexer *lexer) {
    Token token = {0};

    if (!lexer || !lexer->source) {
        token.kind = TOKEN_EOF;
        return token;
    }

    /* Skip whitespace and comments */
    while (lexer->pos < lexer->length) {
        skip_whitespace(lexer);
        if (peek(lexer) == '/' && peek_next(lexer) == '/') {
            skip_comment(lexer);
        } else {
            break;
        }
    }

    if (lexer->pos >= lexer->length) {
        token.kind = TOKEN_EOF;
        token.line = lexer->line;
        token.column = lexer->column;
        return token;
    }

    int start_line = lexer->line;
    int start_column = lexer->column;
    char c = advance(lexer);

    /* Identifiers and keywords */
    if (is_alpha(c)) {
        return parse_identifier(lexer, start_line, start_column);
    }

    /* Numbers */
    if (is_digit(c)) {
        return parse_number(lexer, start_line, start_column);
    }

    /* Handle operators and special characters */
    switch (c) {
        case '(': return make_token(lexer, TOKEN_LPAREN, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case ')': return make_token(lexer, TOKEN_RPAREN, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case '{': return make_token(lexer, TOKEN_LBRACE, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case '}': return make_token(lexer, TOKEN_RBRACE, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case '[': return make_token(lexer, TOKEN_LBRACKET, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case ']': return make_token(lexer, TOKEN_RBRACKET, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case ',': return make_token(lexer, TOKEN_COMMA, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case ';': return make_token(lexer, TOKEN_SEMICOLON, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case ':': return make_token(lexer, TOKEN_COLON, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case '.': return make_token(lexer, TOKEN_DOT, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case '+': return make_token(lexer, TOKEN_PLUS, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case '*': return make_token(lexer, TOKEN_STAR, &lexer->source[lexer->pos - 1], 1, start_line, start_column);
        case '%': return make_token(lexer, TOKEN_PERCENT, &lexer->source[lexer->pos - 1], 1, start_line, start_column);

        case '-':
            /* Check if this is a negative number or minus operator */
            if (is_digit(peek(lexer))) {
                return parse_number(lexer, start_line, start_column);
            }
            return make_token(lexer, TOKEN_MINUS, &lexer->source[lexer->pos - 1], 1, start_line, start_column);

        case '/':
            /* Already handled comments in skip phase, so this is division */
            return make_token(lexer, TOKEN_SLASH, &lexer->source[lexer->pos - 1], 1, start_line, start_column);

        case '=':
            if (match(lexer, '=')) {
                return make_token(lexer, TOKEN_EQ, &lexer->source[lexer->pos - 2], 2, start_line, start_column);
            }
            return make_token(lexer, TOKEN_ASSIGN, &lexer->source[lexer->pos - 1], 1, start_line, start_column);

        case '!':
            if (match(lexer, '=')) {
                return make_token(lexer, TOKEN_NE, &lexer->source[lexer->pos - 2], 2, start_line, start_column);
            }
            return make_token(lexer, TOKEN_NOT, &lexer->source[lexer->pos - 1], 1, start_line, start_column);

        case '<':
            if (match(lexer, '=')) {
                return make_token(lexer, TOKEN_LE, &lexer->source[lexer->pos - 2], 2, start_line, start_column);
            }
            return make_token(lexer, TOKEN_LT, &lexer->source[lexer->pos - 1], 1, start_line, start_column);

        case '>':
            if (match(lexer, '=')) {
                return make_token(lexer, TOKEN_GE, &lexer->source[lexer->pos - 2], 2, start_line, start_column);
            }
            return make_token(lexer, TOKEN_GT, &lexer->source[lexer->pos - 1], 1, start_line, start_column);

        case '&':
            if (match(lexer, '&')) {
                return make_token(lexer, TOKEN_AND, &lexer->source[lexer->pos - 2], 2, start_line, start_column);
            }
            return error_token(lexer, "Unexpected character '&'", start_line, start_column);

        case '|':
            if (match(lexer, '|')) {
                return make_token(lexer, TOKEN_OR, &lexer->source[lexer->pos - 2], 2, start_line, start_column);
            }
            return error_token(lexer, "Unexpected character '|'", start_line, start_column);

        case '"':
            return parse_string(lexer, start_line, start_column);

        default:
            return error_token(lexer, "Unexpected character", start_line, start_column);
    }
}
