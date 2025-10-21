#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ASTNode *ast_create_program(void) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_PROGRAM;
    node->data.program.statements = NULL;
    node->data.program.statement_count = 0;
    return node;
}

ASTNode *ast_create_int_literal(long value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_INT_LITERAL;
    node->data.int_literal.value = value;
    return node;
}

ASTNode *ast_create_float_literal(double value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_FLOAT_LITERAL;
    node->data.float_literal.value = value;
    return node;
}

ASTNode *ast_create_string_literal(const char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_STRING_LITERAL;
    node->data.string_literal.value = (char *)malloc(strlen(value) + 1);
    strcpy(node->data.string_literal.value, value);
    return node;
}

ASTNode *ast_create_bool_literal(int value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_BOOL_LITERAL;
    node->data.bool_literal.value = value ? 1 : 0;
    return node;
}

ASTNode *ast_create_identifier(const char *name) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_IDENTIFIER;
    node->data.identifier.name = (char *)malloc(strlen(name) + 1);
    strcpy(node->data.identifier.name, name);
    return node;
}

ASTNode *ast_create_binary_op(ASTNode *left, ASTNode *right, OperatorType op) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_BINARY_OP;
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    node->data.binary_op.op = op;
    return node;
}

ASTNode *ast_create_unary_op(ASTNode *operand, OperatorType op) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_UNARY_OP;
    node->data.unary_op.operand = operand;
    node->data.unary_op.op = op;
    return node;
}

ASTNode *ast_create_call(ASTNode *function, ASTNode **arguments, size_t arg_count) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_CALL;
    node->data.call.function = function;
    node->data.call.arguments = arguments;
    node->data.call.argument_count = arg_count;
    return node;
}

ASTNode *ast_create_if(ASTNode *condition, ASTNode **then_branch, size_t then_count,
                       ASTNode **else_branch, size_t else_count) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_IF;
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.then_count = then_count;
    node->data.if_stmt.else_branch = else_branch;
    node->data.if_stmt.else_count = else_count;
    return node;
}

ASTNode *ast_create_while(ASTNode *condition, ASTNode **body, size_t body_count) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_WHILE;
    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;
    node->data.while_stmt.body_count = body_count;
    return node;
}

ASTNode *ast_create_function_def(const char *name, char **parameters, size_t param_count,
                                 ASTNode **body, size_t body_count) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_FUNCTION_DEF;
    node->data.function_def.name = (char *)malloc(strlen(name) + 1);
    strcpy(node->data.function_def.name, name);
    node->data.function_def.parameters = parameters;
    node->data.function_def.param_count = param_count;
    node->data.function_def.body = body;
    node->data.function_def.body_count = body_count;
    return node;
}

ASTNode *ast_create_return(ASTNode *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_RETURN;
    node->data.return_stmt.value = value;
    return node;
}

ASTNode *ast_create_var_decl(const char *name, ASTNode *initializer, int is_const) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_VAR_DECL;
    node->data.var_decl.name = (char *)malloc(strlen(name) + 1);
    strcpy(node->data.var_decl.name, name);
    node->data.var_decl.initializer = initializer;
    node->data.var_decl.is_const = is_const;
    return node;
}

ASTNode *ast_create_block(ASTNode **statements, size_t statement_count) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_BLOCK;
    node->data.block.statements = statements;
    node->data.block.statement_count = statement_count;
    return node;
}

ASTNode *ast_create_expr_stmt(ASTNode *expression) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = NODE_EXPRESSION_STMT;
    node->data.expr_stmt.expression = expression;
    return node;
}

void ast_program_add_statement(ASTNode *program, ASTNode *statement) {
    if (!program || program->type != NODE_PROGRAM) {
        return;
    }

    size_t new_count = program->data.program.statement_count + 1;
    ASTNode **new_statements = (ASTNode **)realloc(
        program->data.program.statements,
        sizeof(ASTNode *) * new_count
    );

    if (!new_statements) {
        return;
    }

    new_statements[new_count - 1] = statement;
    program->data.program.statements = new_statements;
    program->data.program.statement_count = new_count;
}

void ast_destroy(ASTNode *node) {
    if (!node) {
        return;
    }

    switch (node->type) {
        case NODE_PROGRAM:
            for (size_t i = 0; i < node->data.program.statement_count; i++) {
                ast_destroy(node->data.program.statements[i]);
            }
            free(node->data.program.statements);
            break;
        case NODE_STRING_LITERAL:
            free(node->data.string_literal.value);
            break;
        case NODE_IDENTIFIER:
            free(node->data.identifier.name);
            break;
        case NODE_BINARY_OP:
            ast_destroy(node->data.binary_op.left);
            ast_destroy(node->data.binary_op.right);
            break;
        case NODE_UNARY_OP:
            ast_destroy(node->data.unary_op.operand);
            break;
        case NODE_CALL:
            ast_destroy(node->data.call.function);
            for (size_t i = 0; i < node->data.call.argument_count; i++) {
                ast_destroy(node->data.call.arguments[i]);
            }
            free(node->data.call.arguments);
            break;
        case NODE_IF:
            ast_destroy(node->data.if_stmt.condition);
            for (size_t i = 0; i < node->data.if_stmt.then_count; i++) {
                ast_destroy(node->data.if_stmt.then_branch[i]);
            }
            free(node->data.if_stmt.then_branch);
            for (size_t i = 0; i < node->data.if_stmt.else_count; i++) {
                ast_destroy(node->data.if_stmt.else_branch[i]);
            }
            free(node->data.if_stmt.else_branch);
            break;
        case NODE_WHILE:
            ast_destroy(node->data.while_stmt.condition);
            for (size_t i = 0; i < node->data.while_stmt.body_count; i++) {
                ast_destroy(node->data.while_stmt.body[i]);
            }
            free(node->data.while_stmt.body);
            break;
        case NODE_FUNCTION_DEF:
            free(node->data.function_def.name);
            for (size_t i = 0; i < node->data.function_def.param_count; i++) {
                free(node->data.function_def.parameters[i]);
            }
            free(node->data.function_def.parameters);
            for (size_t i = 0; i < node->data.function_def.body_count; i++) {
                ast_destroy(node->data.function_def.body[i]);
            }
            free(node->data.function_def.body);
            break;
        case NODE_RETURN:
            ast_destroy(node->data.return_stmt.value);
            break;
        case NODE_VAR_DECL:
            free(node->data.var_decl.name);
            ast_destroy(node->data.var_decl.initializer);
            break;
        case NODE_BLOCK:
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                ast_destroy(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
            break;
        case NODE_EXPRESSION_STMT:
            ast_destroy(node->data.expr_stmt.expression);
            break;
        default:
            break;
    }

    free(node);
}

/* Helper to print indentation */
static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

/* Helper to print operator */
static const char *op_to_string(OperatorType op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_MOD: return "%";
        case OP_EQ: return "==";
        case OP_NE: return "!=";
        case OP_LT: return "<";
        case OP_LE: return "<=";
        case OP_GT: return ">";
        case OP_GE: return ">=";
        case OP_AND: return "&&";
        case OP_OR: return "||";
        case OP_NOT: return "!";
        case OP_ASSIGN: return "=";
        default: return "?";
    }
}

/* Print AST for debugging/visualization */
void ast_print(ASTNode *node, int indent) {
    if (!node) {
        print_indent(indent);
        printf("(null)\n");
        return;
    }

    print_indent(indent);

    switch (node->type) {
        case NODE_PROGRAM:
            printf("PROGRAM [line %d]\n", node->line);
            for (size_t i = 0; i < node->data.program.statement_count; i++) {
                ast_print(node->data.program.statements[i], indent + 1);
            }
            break;

        case NODE_INT_LITERAL:
            printf("INT: %ld [line %d]\n", node->data.int_literal.value, node->line);
            break;

        case NODE_FLOAT_LITERAL:
            printf("FLOAT: %f [line %d]\n", node->data.float_literal.value, node->line);
            break;

        case NODE_STRING_LITERAL:
            printf("STRING: \"%s\" [line %d]\n", node->data.string_literal.value, node->line);
            break;

        case NODE_BOOL_LITERAL:
            printf("BOOL: %s [line %d]\n",
                   node->data.bool_literal.value ? "true" : "false", node->line);
            break;

        case NODE_IDENTIFIER:
            printf("IDENTIFIER: %s [line %d]\n", node->data.identifier.name, node->line);
            break;

        case NODE_BINARY_OP:
            printf("BINARY_OP: %s [line %d]\n",
                   op_to_string(node->data.binary_op.op), node->line);
            ast_print(node->data.binary_op.left, indent + 1);
            ast_print(node->data.binary_op.right, indent + 1);
            break;

        case NODE_UNARY_OP:
            printf("UNARY_OP: %s [line %d]\n",
                   op_to_string(node->data.unary_op.op), node->line);
            ast_print(node->data.unary_op.operand, indent + 1);
            break;

        case NODE_CALL:
            printf("CALL [line %d]\n", node->line);
            print_indent(indent + 1);
            printf("Function:\n");
            ast_print(node->data.call.function, indent + 2);
            print_indent(indent + 1);
            printf("Arguments (%zu):\n", node->data.call.argument_count);
            for (size_t i = 0; i < node->data.call.argument_count; i++) {
                ast_print(node->data.call.arguments[i], indent + 2);
            }
            break;

        case NODE_IF:
            printf("IF [line %d]\n", node->line);
            print_indent(indent + 1);
            printf("Condition:\n");
            ast_print(node->data.if_stmt.condition, indent + 2);
            print_indent(indent + 1);
            printf("Then (%zu statements):\n", node->data.if_stmt.then_count);
            for (size_t i = 0; i < node->data.if_stmt.then_count; i++) {
                ast_print(node->data.if_stmt.then_branch[i], indent + 2);
            }
            if (node->data.if_stmt.else_count > 0) {
                print_indent(indent + 1);
                printf("Else (%zu statements):\n", node->data.if_stmt.else_count);
                for (size_t i = 0; i < node->data.if_stmt.else_count; i++) {
                    ast_print(node->data.if_stmt.else_branch[i], indent + 2);
                }
            }
            break;

        case NODE_WHILE:
            printf("WHILE [line %d]\n", node->line);
            print_indent(indent + 1);
            printf("Condition:\n");
            ast_print(node->data.while_stmt.condition, indent + 2);
            print_indent(indent + 1);
            printf("Body (%zu statements):\n", node->data.while_stmt.body_count);
            for (size_t i = 0; i < node->data.while_stmt.body_count; i++) {
                ast_print(node->data.while_stmt.body[i], indent + 2);
            }
            break;

        case NODE_FUNCTION_DEF:
            printf("FUNCTION: %s [line %d]\n", node->data.function_def.name, node->line);
            print_indent(indent + 1);
            printf("Parameters (%zu): ", node->data.function_def.param_count);
            for (size_t i = 0; i < node->data.function_def.param_count; i++) {
                printf("%s%s", i > 0 ? ", " : "", node->data.function_def.parameters[i]);
            }
            printf("\n");
            print_indent(indent + 1);
            printf("Body (%zu statements):\n", node->data.function_def.body_count);
            for (size_t i = 0; i < node->data.function_def.body_count; i++) {
                ast_print(node->data.function_def.body[i], indent + 2);
            }
            break;

        case NODE_RETURN:
            printf("RETURN [line %d]\n", node->line);
            if (node->data.return_stmt.value) {
                ast_print(node->data.return_stmt.value, indent + 1);
            }
            break;

        case NODE_VAR_DECL:
            printf("VAR_DECL: %s (%s) [line %d]\n",
                   node->data.var_decl.name,
                   node->data.var_decl.is_const ? "const" : "let",
                   node->line);
            print_indent(indent + 1);
            printf("Initializer:\n");
            ast_print(node->data.var_decl.initializer, indent + 2);
            break;

        case NODE_BLOCK:
            printf("BLOCK (%zu statements) [line %d]\n",
                   node->data.block.statement_count, node->line);
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                ast_print(node->data.block.statements[i], indent + 1);
            }
            break;

        case NODE_EXPRESSION_STMT:
            printf("EXPR_STMT [line %d]\n", node->line);
            ast_print(node->data.expr_stmt.expression, indent + 1);
            break;

        default:
            printf("UNKNOWN NODE TYPE %d [line %d]\n", node->type, node->line);
            break;
    }
}
