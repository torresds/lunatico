#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include <stdlib.h>


typedef enum { TVAR, TPRIM, TFUN } TypeKind;

typedef struct Type {
    TypeKind kind;
    int var_id;            // TVAR
    DataType prim;         // TPRIM
    struct Type *arg, *ret; // TFUN
    struct Type *instance; 
} Type;

typedef struct {
    int *vars;
    int var_count;
    Type *type;
} TypeScheme;

typedef struct EnvEntry {
    char *name;
    TypeScheme *scheme;
    struct EnvEntry *next;
} EnvEntry;

void semantic_check(ASTNode *root);

#endif