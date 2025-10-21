#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef enum {
    NODE_PROGRAM,
    NODE_EXPRESSION_STMT,
    NODE_INT_LITERAL,
    NODE_FLOAT_LITERAL,
    NODE_STRING_LITERAL,
    NODE_BOOL_LITERAL,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_CALL,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_FUNCTION_DEF,
    NODE_RETURN,
    NODE_VAR_DECL,
    NODE_BLOCK,
} NodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EQ,
    OP_NE,
    OP_LT,
    OP_LE,
    OP_GT,
    OP_GE,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_ASSIGN,
} OperatorType;

typedef struct ASTNode {
    NodeType type;
    int line;
    union {
        struct {
            struct ASTNode **statements;
            size_t statement_count;
        } program;
        struct {
            long value;
        } int_literal;
        struct {
            double value;
        } float_literal;
        struct {
            char *value;
        } string_literal;
        struct {
            int value;
        } bool_literal;
        struct {
            char *name;
        } identifier;
        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            OperatorType op;
        } binary_op;
        struct {
            struct ASTNode *operand;
            OperatorType op;
        } unary_op;
        struct {
            struct ASTNode *function;
            struct ASTNode **arguments;
            size_t argument_count;
        } call;
        struct {
            struct ASTNode *condition;
            struct ASTNode **then_branch;
            size_t then_count;
            struct ASTNode **else_branch;
            size_t else_count;
        } if_stmt;
        struct {
            struct ASTNode *condition;
            struct ASTNode **body;
            size_t body_count;
        } while_stmt;
        struct {
            char *name;
            char **parameters;
            size_t param_count;
            struct ASTNode **body;
            size_t body_count;
        } function_def;
        struct {
            struct ASTNode *value;
        } return_stmt;
        struct {
            char *name;
            struct ASTNode *initializer;
            int is_const;
        } var_decl;
        struct {
            struct ASTNode **statements;
            size_t statement_count;
        } block;
        struct {
            struct ASTNode *expression;
        } expr_stmt;
    } data;
} ASTNode;

ASTNode *ast_create_program(void);
ASTNode *ast_create_int_literal(long value);
ASTNode *ast_create_float_literal(double value);
ASTNode *ast_create_string_literal(const char *value);
ASTNode *ast_create_bool_literal(int value);
ASTNode *ast_create_identifier(const char *name);
ASTNode *ast_create_binary_op(ASTNode *left, ASTNode *right, OperatorType op);
ASTNode *ast_create_unary_op(ASTNode *operand, OperatorType op);
ASTNode *ast_create_call(ASTNode *function, ASTNode **arguments, size_t arg_count);
ASTNode *ast_create_if(ASTNode *condition, ASTNode **then_branch, size_t then_count,
                       ASTNode **else_branch, size_t else_count);
ASTNode *ast_create_while(ASTNode *condition, ASTNode **body, size_t body_count);
ASTNode *ast_create_function_def(const char *name, char **parameters, size_t param_count,
                                 ASTNode **body, size_t body_count);
ASTNode *ast_create_return(ASTNode *value);
ASTNode *ast_create_var_decl(const char *name, ASTNode *initializer, int is_const);
ASTNode *ast_create_block(ASTNode **statements, size_t statement_count);
ASTNode *ast_create_expr_stmt(ASTNode *expression);
void ast_program_add_statement(ASTNode *program, ASTNode *statement);
void ast_destroy(ASTNode *node);
void ast_print(ASTNode *node, int indent);

#endif
