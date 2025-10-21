#include "codegen.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct CodeGen {
    FILE *output;
    int indent_level;
    bool in_function;
    ASTNode **functions;
    size_t function_count;
    size_t function_capacity;
} CodeGen;

/* Forward declarations of helper functions */
static void emit_indent(CodeGen *gen);
static void emit_includes(CodeGen *gen);
static void emit_forward_declarations(CodeGen *gen);
static void emit_expression(CodeGen *gen, ASTNode *node);
static void emit_statement(CodeGen *gen, ASTNode *node);
static void emit_statement_list(CodeGen *gen, ASTNode **statements, size_t count);
static void emit_function_definition(CodeGen *gen, ASTNode *node);
static void emit_binary_operator(CodeGen *gen, OperatorType op);
static void emit_unary_operator(CodeGen *gen, OperatorType op);
static void collect_functions(CodeGen *gen, ASTNode *ast);
static void add_function(CodeGen *gen, ASTNode *func);
static bool has_top_level_statements(ASTNode *ast);

CodeGen *codegen_create(FILE *output) {
    CodeGen *gen = malloc(sizeof(CodeGen));
    if (!gen) {
        return NULL;
    }
    gen->output = output;
    gen->indent_level = 0;
    gen->in_function = false;
    gen->functions = NULL;
    gen->function_count = 0;
    gen->function_capacity = 0;
    return gen;
}

void codegen_destroy(CodeGen *gen) {
    if (gen) {
        free(gen->functions);
        free(gen);
    }
}

/* Main code generation entry point */
void codegen_generate(CodeGen *gen, ASTNode *ast) {
    if (!gen || !ast) {
        return;
    }

    if (ast->type != NODE_PROGRAM) {
        return;
    }

    /* First pass: collect all function definitions */
    collect_functions(gen, ast);

    /* Emit includes */
    emit_includes(gen);
    fprintf(gen->output, "\n");

    /* Emit forward declarations for all functions */
    emit_forward_declarations(gen);
    if (gen->function_count > 0) {
        fprintf(gen->output, "\n");
    }

    /* Emit all function definitions */
    for (size_t i = 0; i < gen->function_count; i++) {
        emit_function_definition(gen, gen->functions[i]);
        fprintf(gen->output, "\n");
    }

    /* Check if we have top-level statements (non-function statements) */
    if (has_top_level_statements(ast)) {
        /* Generate main function with top-level statements */
        fprintf(gen->output, "int main(void) {\n");
        gen->indent_level++;
        gen->in_function = true;

        for (size_t i = 0; i < ast->data.program.statement_count; i++) {
            ASTNode *stmt = ast->data.program.statements[i];
            /* Skip function definitions - they're already emitted */
            if (stmt->type != NODE_FUNCTION_DEF) {
                emit_statement(gen, stmt);
            }
        }

        emit_indent(gen);
        fprintf(gen->output, "return 0;\n");
        gen->indent_level--;
        gen->in_function = false;
        fprintf(gen->output, "}\n");
    }
}

/* Collect all function definitions from the program */
static void collect_functions(CodeGen *gen, ASTNode *ast) {
    if (ast->type != NODE_PROGRAM) {
        return;
    }

    for (size_t i = 0; i < ast->data.program.statement_count; i++) {
        ASTNode *stmt = ast->data.program.statements[i];
        if (stmt->type == NODE_FUNCTION_DEF) {
            add_function(gen, stmt);
        }
    }
}

/* Add a function to the functions list */
static void add_function(CodeGen *gen, ASTNode *func) {
    if (gen->function_count >= gen->function_capacity) {
        size_t new_capacity = gen->function_capacity == 0 ? 8 : gen->function_capacity * 2;
        ASTNode **new_functions = realloc(gen->functions, new_capacity * sizeof(ASTNode *));
        if (!new_functions) {
            return;
        }
        gen->functions = new_functions;
        gen->function_capacity = new_capacity;
    }
    gen->functions[gen->function_count++] = func;
}

/* Check if there are any top-level statements that aren't function definitions */
static bool has_top_level_statements(ASTNode *ast) {
    if (ast->type != NODE_PROGRAM) {
        return false;
    }

    for (size_t i = 0; i < ast->data.program.statement_count; i++) {
        if (ast->data.program.statements[i]->type != NODE_FUNCTION_DEF) {
            return true;
        }
    }
    return false;
}

/* Emit indentation */
static void emit_indent(CodeGen *gen) {
    for (int i = 0; i < gen->indent_level; i++) {
        fprintf(gen->output, "    ");
    }
}

/* Emit #include statements */
static void emit_includes(CodeGen *gen) {
    fprintf(gen->output, "#include \"runtime/print.h\"\n");
}

/* Emit forward declarations for all functions */
static void emit_forward_declarations(CodeGen *gen) {
    for (size_t i = 0; i < gen->function_count; i++) {
        ASTNode *func = gen->functions[i];
        fprintf(gen->output, "int %s(", func->data.function_def.name);

        for (size_t j = 0; j < func->data.function_def.param_count; j++) {
            if (j > 0) {
                fprintf(gen->output, ", ");
            }
            fprintf(gen->output, "int %s", func->data.function_def.parameters[j]);
        }

        fprintf(gen->output, ");\n");
    }
}

/* Emit a function definition */
static void emit_function_definition(CodeGen *gen, ASTNode *node) {
    if (node->type != NODE_FUNCTION_DEF) {
        return;
    }

    /* Function signature */
    fprintf(gen->output, "int %s(", node->data.function_def.name);

    for (size_t i = 0; i < node->data.function_def.param_count; i++) {
        if (i > 0) {
            fprintf(gen->output, ", ");
        }
        fprintf(gen->output, "int %s", node->data.function_def.parameters[i]);
    }

    fprintf(gen->output, ") {\n");

    /* Function body */
    gen->indent_level++;
    gen->in_function = true;
    emit_statement_list(gen, node->data.function_def.body, node->data.function_def.body_count);
    gen->in_function = false;
    gen->indent_level--;

    fprintf(gen->output, "}\n");
}

/* Emit a list of statements */
static void emit_statement_list(CodeGen *gen, ASTNode **statements, size_t count) {
    for (size_t i = 0; i < count; i++) {
        emit_statement(gen, statements[i]);
    }
}

/* Emit a single statement */
static void emit_statement(CodeGen *gen, ASTNode *node) {
    if (!node) {
        return;
    }

    switch (node->type) {
        case NODE_EXPRESSION_STMT:
            emit_indent(gen);
            emit_expression(gen, node->data.expr_stmt.expression);
            fprintf(gen->output, ";\n");
            break;

        case NODE_VAR_DECL:
            emit_indent(gen);
            fprintf(gen->output, "int %s", node->data.var_decl.name);
            if (node->data.var_decl.initializer) {
                fprintf(gen->output, " = ");
                emit_expression(gen, node->data.var_decl.initializer);
            }
            fprintf(gen->output, ";\n");
            break;

        case NODE_IF:
            emit_indent(gen);
            fprintf(gen->output, "if (");
            emit_expression(gen, node->data.if_stmt.condition);
            fprintf(gen->output, ") {\n");
            gen->indent_level++;
            emit_statement_list(gen, node->data.if_stmt.then_branch, node->data.if_stmt.then_count);
            gen->indent_level--;
            emit_indent(gen);
            if (node->data.if_stmt.else_count > 0) {
                fprintf(gen->output, "} else {\n");
                gen->indent_level++;
                emit_statement_list(gen, node->data.if_stmt.else_branch, node->data.if_stmt.else_count);
                gen->indent_level--;
                emit_indent(gen);
            }
            fprintf(gen->output, "}\n");
            break;

        case NODE_WHILE:
            emit_indent(gen);
            fprintf(gen->output, "while (");
            emit_expression(gen, node->data.while_stmt.condition);
            fprintf(gen->output, ") {\n");
            gen->indent_level++;
            emit_statement_list(gen, node->data.while_stmt.body, node->data.while_stmt.body_count);
            gen->indent_level--;
            emit_indent(gen);
            fprintf(gen->output, "}\n");
            break;

        case NODE_RETURN:
            emit_indent(gen);
            fprintf(gen->output, "return");
            if (node->data.return_stmt.value) {
                fprintf(gen->output, " ");
                emit_expression(gen, node->data.return_stmt.value);
            }
            fprintf(gen->output, ";\n");
            break;

        case NODE_BLOCK:
            emit_indent(gen);
            fprintf(gen->output, "{\n");
            gen->indent_level++;
            emit_statement_list(gen, node->data.block.statements, node->data.block.statement_count);
            gen->indent_level--;
            emit_indent(gen);
            fprintf(gen->output, "}\n");
            break;

        case NODE_FUNCTION_DEF:
            /* Functions are handled separately, skip in statement context */
            break;

        default:
            break;
    }
}

/* Emit an expression */
static void emit_expression(CodeGen *gen, ASTNode *node) {
    if (!node) {
        return;
    }

    switch (node->type) {
        case NODE_INT_LITERAL:
            fprintf(gen->output, "%ld", node->data.int_literal.value);
            break;

        case NODE_FLOAT_LITERAL:
            fprintf(gen->output, "%f", node->data.float_literal.value);
            break;

        case NODE_STRING_LITERAL:
            fprintf(gen->output, "\"%s\"", node->data.string_literal.value);
            break;

        case NODE_BOOL_LITERAL:
            fprintf(gen->output, "%d", node->data.bool_literal.value ? 1 : 0);
            break;

        case NODE_IDENTIFIER:
            fprintf(gen->output, "%s", node->data.identifier.name);
            break;

        case NODE_BINARY_OP:
            fprintf(gen->output, "(");
            emit_expression(gen, node->data.binary_op.left);
            fprintf(gen->output, " ");
            emit_binary_operator(gen, node->data.binary_op.op);
            fprintf(gen->output, " ");
            emit_expression(gen, node->data.binary_op.right);
            fprintf(gen->output, ")");
            break;

        case NODE_UNARY_OP:
            emit_unary_operator(gen, node->data.unary_op.op);
            fprintf(gen->output, "(");
            emit_expression(gen, node->data.unary_op.operand);
            fprintf(gen->output, ")");
            break;

        case NODE_CALL: {
            /* Special case for 'print' function */
            if (node->data.call.function->type == NODE_IDENTIFIER) {
                const char *func_name = node->data.call.function->data.identifier.name;

                if (strcmp(func_name, "print") == 0) {
                    /* Determine which print function to use based on argument */
                    if (node->data.call.argument_count > 0) {
                        ASTNode *arg = node->data.call.arguments[0];

                        /* Try to determine type from AST node type */
                        switch (arg->type) {
                            case NODE_INT_LITERAL:
                            case NODE_IDENTIFIER:
                            case NODE_BINARY_OP:
                            case NODE_CALL:
                                /* Default to int for now */
                                fprintf(gen->output, "miru_print_int(");
                                emit_expression(gen, arg);
                                fprintf(gen->output, ")");
                                break;

                            case NODE_FLOAT_LITERAL:
                                fprintf(gen->output, "miru_print_float(");
                                emit_expression(gen, arg);
                                fprintf(gen->output, ")");
                                break;

                            case NODE_STRING_LITERAL:
                                fprintf(gen->output, "miru_print_string(");
                                emit_expression(gen, arg);
                                fprintf(gen->output, ")");
                                break;

                            case NODE_BOOL_LITERAL:
                                fprintf(gen->output, "miru_print_bool(");
                                emit_expression(gen, arg);
                                fprintf(gen->output, ")");
                                break;

                            default:
                                /* Default to int */
                                fprintf(gen->output, "miru_print_int(");
                                emit_expression(gen, arg);
                                fprintf(gen->output, ")");
                                break;
                        }
                    }
                } else {
                    /* Regular function call */
                    fprintf(gen->output, "%s(", func_name);
                    for (size_t i = 0; i < node->data.call.argument_count; i++) {
                        if (i > 0) {
                            fprintf(gen->output, ", ");
                        }
                        emit_expression(gen, node->data.call.arguments[i]);
                    }
                    fprintf(gen->output, ")");
                }
            } else {
                /* Function expression (not just identifier) */
                emit_expression(gen, node->data.call.function);
                fprintf(gen->output, "(");
                for (size_t i = 0; i < node->data.call.argument_count; i++) {
                    if (i > 0) {
                        fprintf(gen->output, ", ");
                    }
                    emit_expression(gen, node->data.call.arguments[i]);
                }
                fprintf(gen->output, ")");
            }
            break;
        }

        default:
            break;
    }
}

/* Emit a binary operator */
static void emit_binary_operator(CodeGen *gen, OperatorType op) {
    switch (op) {
        case OP_ADD:
            fprintf(gen->output, "+");
            break;
        case OP_SUB:
            fprintf(gen->output, "-");
            break;
        case OP_MUL:
            fprintf(gen->output, "*");
            break;
        case OP_DIV:
            fprintf(gen->output, "/");
            break;
        case OP_MOD:
            fprintf(gen->output, "%%");
            break;
        case OP_EQ:
            fprintf(gen->output, "==");
            break;
        case OP_NE:
            fprintf(gen->output, "!=");
            break;
        case OP_LT:
            fprintf(gen->output, "<");
            break;
        case OP_LE:
            fprintf(gen->output, "<=");
            break;
        case OP_GT:
            fprintf(gen->output, ">");
            break;
        case OP_GE:
            fprintf(gen->output, ">=");
            break;
        case OP_AND:
            fprintf(gen->output, "&&");
            break;
        case OP_OR:
            fprintf(gen->output, "||");
            break;
        case OP_ASSIGN:
            fprintf(gen->output, "=");
            break;
        default:
            break;
    }
}

/* Emit a unary operator */
static void emit_unary_operator(CodeGen *gen, OperatorType op) {
    switch (op) {
        case OP_NOT:
            fprintf(gen->output, "!");
            break;
        case OP_SUB:
            fprintf(gen->output, "-");
            break;
        default:
            break;
    }
}
