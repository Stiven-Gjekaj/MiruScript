#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer *lexer;
    Token current_token;
    Token peek_token;
} Parser;

Parser *parser_create(Lexer *lexer);
void parser_destroy(Parser *parser);
ASTNode *parser_parse(Parser *parser);

#endif
