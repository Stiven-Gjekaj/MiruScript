#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h>

typedef struct CodeGen CodeGen;

CodeGen *codegen_create(FILE *output);
void codegen_destroy(CodeGen *gen);
void codegen_generate(CodeGen *gen, ASTNode *ast);

#endif
