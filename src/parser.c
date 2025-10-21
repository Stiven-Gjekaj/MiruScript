#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Helper to duplicate a string with length (strndup is not standard C) */
static char *string_dup_len(const char *s, size_t len) {
    char *result = (char *)malloc(len + 1);
    if (result) {
        memcpy(result, s, len);
        result[len] = '\0';
    }
    return result;
}

/* Forward declarations */
static ASTNode *parse_statement(Parser *parser);
static ASTNode *parse_expression(Parser *parser);
static ASTNode *parse_assignment(Parser *parser);
static ASTNode *parse_logical_or(Parser *parser);
static ASTNode *parse_logical_and(Parser *parser);
static ASTNode *parse_equality(Parser *parser);
static ASTNode *parse_comparison(Parser *parser);
static ASTNode *parse_term(Parser *parser);
static ASTNode *parse_factor(Parser *parser);
static ASTNode *parse_unary(Parser *parser);
static ASTNode *parse_call(Parser *parser);
static ASTNode *parse_primary(Parser *parser);
static ASTNode *parse_var_decl(Parser *parser);
static ASTNode *parse_func_decl(Parser *parser);
static ASTNode *parse_if_stmt(Parser *parser);
static ASTNode *parse_while_stmt(Parser *parser);
static ASTNode *parse_return_stmt(Parser *parser);
static ASTNode *parse_expr_stmt(Parser *parser);

/* Helper functions */
static Token advance(Parser *parser) {
    Token current = parser->current_token;
    parser->current_token = parser->peek_token;
    parser->peek_token = lexer_next_token(parser->lexer);
    return current;
}

static Token peek(Parser *parser) {
    return parser->current_token;
}

static int check(Parser *parser, TokenKind kind) {
    return parser->current_token.kind == kind;
}

static int match(Parser *parser, TokenKind kind) {
    if (check(parser, kind)) {
        advance(parser);
        return 1;
    }
    return 0;
}

static Token expect(Parser *parser, TokenKind kind, const char *message) {
    if (check(parser, kind)) {
        return advance(parser);
    }

    fprintf(stderr, "Parse error at line %d: %s\n",
            parser->current_token.line, message);

    Token error_token = {0};
    error_token.kind = TOKEN_ERROR;
    return error_token;
}

static void report_error(Parser *parser, const char *message) {
    fprintf(stderr, "Parse error at line %d: %s\n",
            parser->current_token.line, message);
}

/* Parser creation and destruction */
Parser *parser_create(Lexer *lexer) {
    Parser *parser = malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    parser->peek_token = lexer_next_token(lexer);
    return parser;
}

void parser_destroy(Parser *parser) {
    if (parser) {
        free(parser);
    }
}

/* Main parsing function */
ASTNode *parser_parse(Parser *parser) {
    if (!parser) {
        return NULL;
    }

    ASTNode *program = ast_create_program();
    if (!program) {
        return NULL;
    }

    program->line = 1;

    while (!check(parser, TOKEN_EOF)) {
        ASTNode *stmt = parse_statement(parser);
        if (!stmt) {
            ast_destroy(program);
            return NULL;
        }
        ast_program_add_statement(program, stmt);
    }

    return program;
}

/* Statement parsing */
static ASTNode *parse_statement(Parser *parser) {
    if (match(parser, TOKEN_LET) || match(parser, TOKEN_CONST)) {
        /* Backtrack - we need to know if it was let or const */
        parser->peek_token = parser->current_token;
        parser->current_token = peek(parser);
        if (parser->current_token.kind == TOKEN_LET ||
            parser->current_token.kind == TOKEN_CONST) {
            return parse_var_decl(parser);
        }
    }

    if (check(parser, TOKEN_LET) || check(parser, TOKEN_CONST)) {
        return parse_var_decl(parser);
    }

    if (check(parser, TOKEN_FUNC)) {
        return parse_func_decl(parser);
    }

    if (check(parser, TOKEN_IF)) {
        return parse_if_stmt(parser);
    }

    if (check(parser, TOKEN_WHILE)) {
        return parse_while_stmt(parser);
    }

    if (check(parser, TOKEN_RETURN)) {
        return parse_return_stmt(parser);
    }

    return parse_expr_stmt(parser);
}

/* Variable declaration: ("let" | "const") IDENTIFIER "=" expression ";" */
static ASTNode *parse_var_decl(Parser *parser) {
    int is_const = 0;
    int line = parser->current_token.line;

    if (match(parser, TOKEN_CONST)) {
        is_const = 1;
    } else if (!match(parser, TOKEN_LET)) {
        report_error(parser, "Expected 'let' or 'const'");
        return NULL;
    }

    Token name_token = expect(parser, TOKEN_IDENTIFIER, "Expected identifier");
    if (name_token.kind == TOKEN_ERROR) {
        return NULL;
    }

    char *name = string_dup_len(name_token.lexeme, name_token.length);

    if (!match(parser, TOKEN_ASSIGN)) {
        report_error(parser, "Expected '=' in variable declaration");
        free(name);
        return NULL;
    }

    ASTNode *initializer = parse_expression(parser);
    if (!initializer) {
        free(name);
        return NULL;
    }

    if (!match(parser, TOKEN_SEMICOLON)) {
        report_error(parser, "Expected ';' after variable declaration");
        free(name);
        ast_destroy(initializer);
        return NULL;
    }

    ASTNode *node = ast_create_var_decl(name, initializer, is_const);
    node->line = line;
    free(name);
    return node;
}

/* Function declaration: "func" IDENTIFIER "(" parameters? ")" "{" statement* "}" */
static ASTNode *parse_func_decl(Parser *parser) {
    int line = parser->current_token.line;

    if (!match(parser, TOKEN_FUNC)) {
        report_error(parser, "Expected 'func'");
        return NULL;
    }

    Token name_token = expect(parser, TOKEN_IDENTIFIER, "Expected function name");
    if (name_token.kind == TOKEN_ERROR) {
        return NULL;
    }

    char *name = string_dup_len(name_token.lexeme, name_token.length);

    if (!match(parser, TOKEN_LPAREN)) {
        report_error(parser, "Expected '(' after function name");
        free(name);
        return NULL;
    }

    /* Parse parameters */
    char **parameters = NULL;
    size_t param_count = 0;

    if (!check(parser, TOKEN_RPAREN)) {
        do {
            Token param_token = expect(parser, TOKEN_IDENTIFIER, "Expected parameter name");
            if (param_token.kind == TOKEN_ERROR) {
                for (size_t i = 0; i < param_count; i++) {
                    free(parameters[i]);
                }
                free(parameters);
                free(name);
                return NULL;
            }

            char **new_params = realloc(parameters, sizeof(char *) * (param_count + 1));
            if (!new_params) {
                for (size_t i = 0; i < param_count; i++) {
                    free(parameters[i]);
                }
                free(parameters);
                free(name);
                return NULL;
            }
            parameters = new_params;
            parameters[param_count] = string_dup_len(param_token.lexeme, param_token.length);
            param_count++;

        } while (match(parser, TOKEN_COMMA));
    }

    if (!match(parser, TOKEN_RPAREN)) {
        report_error(parser, "Expected ')' after parameters");
        for (size_t i = 0; i < param_count; i++) {
            free(parameters[i]);
        }
        free(parameters);
        free(name);
        return NULL;
    }

    if (!match(parser, TOKEN_LBRACE)) {
        report_error(parser, "Expected '{' before function body");
        for (size_t i = 0; i < param_count; i++) {
            free(parameters[i]);
        }
        free(parameters);
        free(name);
        return NULL;
    }

    /* Parse body */
    ASTNode **body = NULL;
    size_t body_count = 0;

    while (!check(parser, TOKEN_RBRACE) && !check(parser, TOKEN_EOF)) {
        ASTNode *stmt = parse_statement(parser);
        if (!stmt) {
            for (size_t i = 0; i < param_count; i++) {
                free(parameters[i]);
            }
            free(parameters);
            for (size_t i = 0; i < body_count; i++) {
                ast_destroy(body[i]);
            }
            free(body);
            free(name);
            return NULL;
        }

        ASTNode **new_body = realloc(body, sizeof(ASTNode *) * (body_count + 1));
        if (!new_body) {
            ast_destroy(stmt);
            for (size_t i = 0; i < param_count; i++) {
                free(parameters[i]);
            }
            free(parameters);
            for (size_t i = 0; i < body_count; i++) {
                ast_destroy(body[i]);
            }
            free(body);
            free(name);
            return NULL;
        }
        body = new_body;
        body[body_count] = stmt;
        body_count++;
    }

    if (!match(parser, TOKEN_RBRACE)) {
        report_error(parser, "Expected '}' after function body");
        for (size_t i = 0; i < param_count; i++) {
            free(parameters[i]);
        }
        free(parameters);
        for (size_t i = 0; i < body_count; i++) {
            ast_destroy(body[i]);
        }
        free(body);
        free(name);
        return NULL;
    }

    ASTNode *node = ast_create_function_def(name, parameters, param_count, body, body_count);
    node->line = line;
    free(name);
    return node;
}

/* If statement: "if" "(" expression ")" "{" statement* "}" ("else" "{" statement* "}")? */
static ASTNode *parse_if_stmt(Parser *parser) {
    int line = parser->current_token.line;

    if (!match(parser, TOKEN_IF)) {
        report_error(parser, "Expected 'if'");
        return NULL;
    }

    if (!match(parser, TOKEN_LPAREN)) {
        report_error(parser, "Expected '(' after 'if'");
        return NULL;
    }

    ASTNode *condition = parse_expression(parser);
    if (!condition) {
        return NULL;
    }

    if (!match(parser, TOKEN_RPAREN)) {
        report_error(parser, "Expected ')' after if condition");
        ast_destroy(condition);
        return NULL;
    }

    if (!match(parser, TOKEN_LBRACE)) {
        report_error(parser, "Expected '{' after if condition");
        ast_destroy(condition);
        return NULL;
    }

    /* Parse then branch */
    ASTNode **then_branch = NULL;
    size_t then_count = 0;

    while (!check(parser, TOKEN_RBRACE) && !check(parser, TOKEN_EOF)) {
        ASTNode *stmt = parse_statement(parser);
        if (!stmt) {
            ast_destroy(condition);
            for (size_t i = 0; i < then_count; i++) {
                ast_destroy(then_branch[i]);
            }
            free(then_branch);
            return NULL;
        }

        ASTNode **new_then = realloc(then_branch, sizeof(ASTNode *) * (then_count + 1));
        if (!new_then) {
            ast_destroy(stmt);
            ast_destroy(condition);
            for (size_t i = 0; i < then_count; i++) {
                ast_destroy(then_branch[i]);
            }
            free(then_branch);
            return NULL;
        }
        then_branch = new_then;
        then_branch[then_count] = stmt;
        then_count++;
    }

    if (!match(parser, TOKEN_RBRACE)) {
        report_error(parser, "Expected '}' after then branch");
        ast_destroy(condition);
        for (size_t i = 0; i < then_count; i++) {
            ast_destroy(then_branch[i]);
        }
        free(then_branch);
        return NULL;
    }

    /* Parse optional else branch */
    ASTNode **else_branch = NULL;
    size_t else_count = 0;

    if (match(parser, TOKEN_ELSE)) {
        if (!match(parser, TOKEN_LBRACE)) {
            report_error(parser, "Expected '{' after 'else'");
            ast_destroy(condition);
            for (size_t i = 0; i < then_count; i++) {
                ast_destroy(then_branch[i]);
            }
            free(then_branch);
            return NULL;
        }

        while (!check(parser, TOKEN_RBRACE) && !check(parser, TOKEN_EOF)) {
            ASTNode *stmt = parse_statement(parser);
            if (!stmt) {
                ast_destroy(condition);
                for (size_t i = 0; i < then_count; i++) {
                    ast_destroy(then_branch[i]);
                }
                free(then_branch);
                for (size_t i = 0; i < else_count; i++) {
                    ast_destroy(else_branch[i]);
                }
                free(else_branch);
                return NULL;
            }

            ASTNode **new_else = realloc(else_branch, sizeof(ASTNode *) * (else_count + 1));
            if (!new_else) {
                ast_destroy(stmt);
                ast_destroy(condition);
                for (size_t i = 0; i < then_count; i++) {
                    ast_destroy(then_branch[i]);
                }
                free(then_branch);
                for (size_t i = 0; i < else_count; i++) {
                    ast_destroy(else_branch[i]);
                }
                free(else_branch);
                return NULL;
            }
            else_branch = new_else;
            else_branch[else_count] = stmt;
            else_count++;
        }

        if (!match(parser, TOKEN_RBRACE)) {
            report_error(parser, "Expected '}' after else branch");
            ast_destroy(condition);
            for (size_t i = 0; i < then_count; i++) {
                ast_destroy(then_branch[i]);
            }
            free(then_branch);
            for (size_t i = 0; i < else_count; i++) {
                ast_destroy(else_branch[i]);
            }
            free(else_branch);
            return NULL;
        }
    }

    ASTNode *node = ast_create_if(condition, then_branch, then_count, else_branch, else_count);
    node->line = line;
    return node;
}

/* While statement: "while" "(" expression ")" "{" statement* "}" */
static ASTNode *parse_while_stmt(Parser *parser) {
    int line = parser->current_token.line;

    if (!match(parser, TOKEN_WHILE)) {
        report_error(parser, "Expected 'while'");
        return NULL;
    }

    if (!match(parser, TOKEN_LPAREN)) {
        report_error(parser, "Expected '(' after 'while'");
        return NULL;
    }

    ASTNode *condition = parse_expression(parser);
    if (!condition) {
        return NULL;
    }

    if (!match(parser, TOKEN_RPAREN)) {
        report_error(parser, "Expected ')' after while condition");
        ast_destroy(condition);
        return NULL;
    }

    if (!match(parser, TOKEN_LBRACE)) {
        report_error(parser, "Expected '{' after while condition");
        ast_destroy(condition);
        return NULL;
    }

    /* Parse body */
    ASTNode **body = NULL;
    size_t body_count = 0;

    while (!check(parser, TOKEN_RBRACE) && !check(parser, TOKEN_EOF)) {
        ASTNode *stmt = parse_statement(parser);
        if (!stmt) {
            ast_destroy(condition);
            for (size_t i = 0; i < body_count; i++) {
                ast_destroy(body[i]);
            }
            free(body);
            return NULL;
        }

        ASTNode **new_body = realloc(body, sizeof(ASTNode *) * (body_count + 1));
        if (!new_body) {
            ast_destroy(stmt);
            ast_destroy(condition);
            for (size_t i = 0; i < body_count; i++) {
                ast_destroy(body[i]);
            }
            free(body);
            return NULL;
        }
        body = new_body;
        body[body_count] = stmt;
        body_count++;
    }

    if (!match(parser, TOKEN_RBRACE)) {
        report_error(parser, "Expected '}' after while body");
        ast_destroy(condition);
        for (size_t i = 0; i < body_count; i++) {
            ast_destroy(body[i]);
        }
        free(body);
        return NULL;
    }

    ASTNode *node = ast_create_while(condition, body, body_count);
    node->line = line;
    return node;
}

/* Return statement: "return" expression? ";" */
static ASTNode *parse_return_stmt(Parser *parser) {
    int line = parser->current_token.line;

    if (!match(parser, TOKEN_RETURN)) {
        report_error(parser, "Expected 'return'");
        return NULL;
    }

    ASTNode *value = NULL;

    if (!check(parser, TOKEN_SEMICOLON)) {
        value = parse_expression(parser);
        if (!value) {
            return NULL;
        }
    }

    if (!match(parser, TOKEN_SEMICOLON)) {
        report_error(parser, "Expected ';' after return statement");
        ast_destroy(value);
        return NULL;
    }

    ASTNode *node = ast_create_return(value);
    node->line = line;
    return node;
}

/* Expression statement: expression ";" */
static ASTNode *parse_expr_stmt(Parser *parser) {
    int line = parser->current_token.line;

    ASTNode *expr = parse_expression(parser);
    if (!expr) {
        return NULL;
    }

    if (!match(parser, TOKEN_SEMICOLON)) {
        report_error(parser, "Expected ';' after expression");
        ast_destroy(expr);
        return NULL;
    }

    ASTNode *node = ast_create_expr_stmt(expr);
    node->line = line;
    return node;
}

/* Expression parsing */
static ASTNode *parse_expression(Parser *parser) {
    return parse_assignment(parser);
}

/* Assignment: IDENTIFIER "=" assignment | logical_or */
static ASTNode *parse_assignment(Parser *parser) {
    ASTNode *expr = parse_logical_or(parser);

    if (match(parser, TOKEN_ASSIGN)) {
        if (!expr || expr->type != NODE_IDENTIFIER) {
            report_error(parser, "Invalid assignment target");
            ast_destroy(expr);
            return NULL;
        }

        int line = parser->current_token.line;
        ASTNode *value = parse_assignment(parser);
        if (!value) {
            ast_destroy(expr);
            return NULL;
        }

        /* Create assignment as binary op with OP_ASSIGN operator */
        ASTNode *node = ast_create_binary_op(expr, value, OP_ASSIGN);
        node->line = line;
        return node;
    }

    return expr;
}

/* Logical OR: logical_and ("||" logical_and)* */
static ASTNode *parse_logical_or(Parser *parser) {
    ASTNode *left = parse_logical_and(parser);
    if (!left) {
        return NULL;
    }

    while (match(parser, TOKEN_OR)) {
        int line = parser->current_token.line;
        ASTNode *right = parse_logical_and(parser);
        if (!right) {
            ast_destroy(left);
            return NULL;
        }

        ASTNode *node = ast_create_binary_op(left, right, OP_OR);
        node->line = line;
        left = node;
    }

    return left;
}

/* Logical AND: equality ("&&" equality)* */
static ASTNode *parse_logical_and(Parser *parser) {
    ASTNode *left = parse_equality(parser);
    if (!left) {
        return NULL;
    }

    while (match(parser, TOKEN_AND)) {
        int line = parser->current_token.line;
        ASTNode *right = parse_equality(parser);
        if (!right) {
            ast_destroy(left);
            return NULL;
        }

        ASTNode *node = ast_create_binary_op(left, right, OP_AND);
        node->line = line;
        left = node;
    }

    return left;
}

/* Equality: comparison (("==" | "!=") comparison)* */
static ASTNode *parse_equality(Parser *parser) {
    ASTNode *left = parse_comparison(parser);
    if (!left) {
        return NULL;
    }

    while (1) {
        OperatorType op;
        int line = parser->current_token.line;

        if (match(parser, TOKEN_EQ)) {
            op = OP_EQ;
        } else if (match(parser, TOKEN_NE)) {
            op = OP_NE;
        } else {
            break;
        }

        ASTNode *right = parse_comparison(parser);
        if (!right) {
            ast_destroy(left);
            return NULL;
        }

        ASTNode *node = ast_create_binary_op(left, right, op);
        node->line = line;
        left = node;
    }

    return left;
}

/* Comparison: term (("<" | "<=" | ">" | ">=") term)* */
static ASTNode *parse_comparison(Parser *parser) {
    ASTNode *left = parse_term(parser);
    if (!left) {
        return NULL;
    }

    while (1) {
        OperatorType op;
        int line = parser->current_token.line;

        if (match(parser, TOKEN_LT)) {
            op = OP_LT;
        } else if (match(parser, TOKEN_LE)) {
            op = OP_LE;
        } else if (match(parser, TOKEN_GT)) {
            op = OP_GT;
        } else if (match(parser, TOKEN_GE)) {
            op = OP_GE;
        } else {
            break;
        }

        ASTNode *right = parse_term(parser);
        if (!right) {
            ast_destroy(left);
            return NULL;
        }

        ASTNode *node = ast_create_binary_op(left, right, op);
        node->line = line;
        left = node;
    }

    return left;
}

/* Term: factor (("+" | "-") factor)* */
static ASTNode *parse_term(Parser *parser) {
    ASTNode *left = parse_factor(parser);
    if (!left) {
        return NULL;
    }

    while (1) {
        OperatorType op;
        int line = parser->current_token.line;

        if (match(parser, TOKEN_PLUS)) {
            op = OP_ADD;
        } else if (match(parser, TOKEN_MINUS)) {
            op = OP_SUB;
        } else {
            break;
        }

        ASTNode *right = parse_factor(parser);
        if (!right) {
            ast_destroy(left);
            return NULL;
        }

        ASTNode *node = ast_create_binary_op(left, right, op);
        node->line = line;
        left = node;
    }

    return left;
}

/* Factor: unary (("*" | "/" | "%") unary)* */
static ASTNode *parse_factor(Parser *parser) {
    ASTNode *left = parse_unary(parser);
    if (!left) {
        return NULL;
    }

    while (1) {
        OperatorType op;
        int line = parser->current_token.line;

        if (match(parser, TOKEN_STAR)) {
            op = OP_MUL;
        } else if (match(parser, TOKEN_SLASH)) {
            op = OP_DIV;
        } else if (match(parser, TOKEN_PERCENT)) {
            op = OP_MOD;
        } else {
            break;
        }

        ASTNode *right = parse_unary(parser);
        if (!right) {
            ast_destroy(left);
            return NULL;
        }

        ASTNode *node = ast_create_binary_op(left, right, op);
        node->line = line;
        left = node;
    }

    return left;
}

/* Unary: ("!" | "-") unary | call */
static ASTNode *parse_unary(Parser *parser) {
    OperatorType op;
    int line = parser->current_token.line;

    if (match(parser, TOKEN_NOT)) {
        op = OP_NOT;
    } else if (match(parser, TOKEN_MINUS)) {
        op = OP_SUB; /* Unary minus uses SUB operator */
    } else {
        return parse_call(parser);
    }

    ASTNode *operand = parse_unary(parser);
    if (!operand) {
        return NULL;
    }

    ASTNode *node = ast_create_unary_op(operand, op);
    node->line = line;
    return node;
}

/* Call: primary ("(" arguments? ")")* */
static ASTNode *parse_call(Parser *parser) {
    ASTNode *expr = parse_primary(parser);
    if (!expr) {
        return NULL;
    }

    while (match(parser, TOKEN_LPAREN)) {
        int line = parser->current_token.line;

        /* Parse arguments */
        ASTNode **arguments = NULL;
        size_t arg_count = 0;

        if (!check(parser, TOKEN_RPAREN)) {
            do {
                ASTNode *arg = parse_expression(parser);
                if (!arg) {
                    for (size_t i = 0; i < arg_count; i++) {
                        ast_destroy(arguments[i]);
                    }
                    free(arguments);
                    ast_destroy(expr);
                    return NULL;
                }

                ASTNode **new_args = realloc(arguments, sizeof(ASTNode *) * (arg_count + 1));
                if (!new_args) {
                    ast_destroy(arg);
                    for (size_t i = 0; i < arg_count; i++) {
                        ast_destroy(arguments[i]);
                    }
                    free(arguments);
                    ast_destroy(expr);
                    return NULL;
                }
                arguments = new_args;
                arguments[arg_count] = arg;
                arg_count++;

            } while (match(parser, TOKEN_COMMA));
        }

        if (!match(parser, TOKEN_RPAREN)) {
            report_error(parser, "Expected ')' after arguments");
            for (size_t i = 0; i < arg_count; i++) {
                ast_destroy(arguments[i]);
            }
            free(arguments);
            ast_destroy(expr);
            return NULL;
        }

        ASTNode *node = ast_create_call(expr, arguments, arg_count);
        node->line = line;
        expr = node;
    }

    return expr;
}

/* Primary: NUMBER | STRING | "true" | "false" | "null" | IDENTIFIER | "(" expression ")" */
static ASTNode *parse_primary(Parser *parser) {
    int line = parser->current_token.line;

    if (check(parser, TOKEN_INT)) {
        Token token = advance(parser);
        long value = token.value.int_value;
        ASTNode *node = ast_create_int_literal(value);
        node->line = line;
        return node;
    }

    if (check(parser, TOKEN_FLOAT)) {
        Token token = advance(parser);
        double value = token.value.float_value;
        ASTNode *node = ast_create_float_literal(value);
        node->line = line;
        return node;
    }

    if (check(parser, TOKEN_STRING)) {
        Token token = advance(parser);
        char *value = string_dup_len(token.lexeme, token.length);
        ASTNode *node = ast_create_string_literal(value);
        node->line = line;
        free(value);
        return node;
    }

    if (match(parser, TOKEN_TRUE)) {
        ASTNode *node = ast_create_bool_literal(1);
        node->line = line;
        return node;
    }

    if (match(parser, TOKEN_FALSE)) {
        ASTNode *node = ast_create_bool_literal(0);
        node->line = line;
        return node;
    }

    if (match(parser, TOKEN_NULL)) {
        /* Represent null as integer 0 for now */
        ASTNode *node = ast_create_int_literal(0);
        node->line = line;
        return node;
    }

    if (check(parser, TOKEN_IDENTIFIER)) {
        Token token = advance(parser);
        char *name = string_dup_len(token.lexeme, token.length);
        ASTNode *node = ast_create_identifier(name);
        node->line = line;
        free(name);
        return node;
    }

    if (match(parser, TOKEN_LPAREN)) {
        ASTNode *expr = parse_expression(parser);
        if (!expr) {
            return NULL;
        }

        if (!match(parser, TOKEN_RPAREN)) {
            report_error(parser, "Expected ')' after expression");
            ast_destroy(expr);
            return NULL;
        }

        return expr;
    }

    report_error(parser, "Unexpected token in expression");
    return NULL;
}
