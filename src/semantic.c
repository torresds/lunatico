#include "semantic.h"
#include "symbol_table.h"
#include <string.h>
#include <stdio.h>

static EnvEntry *env = NULL;
static int next_type_var = 0;

static DataType type_to_datatype(Type *t)
{
    t = prune(t);
    switch (t->kind)
    {
    case TPRIM:
        return t->prim;
    case TFUN:
        return TYPE_FUNCTION;
    default:
        return TYPE_UNKNOWN;
    }
}

static Type *new_type_var(void)
{
    Type *t = malloc(sizeof(Type));
    t->kind = TVAR;
    t->var_id = next_type_var++;
    t->instance = NULL;
    return t;
}

static Type *new_prim(DataType p)
{
    Type *t = malloc(sizeof(Type));
    t->kind = TPRIM;
    t->prim = p;
    t->instance = NULL;
    return t;
}

static Type *new_fun(Type *arg, Type *ret)
{
    Type *t = malloc(sizeof(Type));
    t->kind = TFUN;
    t->arg = arg;
    t->ret = ret;
    t->instance = NULL;
    return t;
}

static Type *prune(Type *t)
{
    if (t->kind == TVAR && t->instance)
    {
        t->instance = prune(t->instance);
        return t->instance;
    }
    return t;
}

static int occurs_in(int id, Type *t)
{
    t = prune(t);
    if (t->kind == TVAR)
        return t->var_id == id;
    if (t->kind == TFUN)
        return occurs_in(id, t->arg) || occurs_in(id, t->ret);
    return 0;
}

static void unify(Type *a, Type *b)
{
    a = prune(a);
    b = prune(b);
    if (a->kind == TVAR)
    {
        if (a != b)
        {
            if (occurs_in(a->var_id, b))
            {
                fprintf(stderr, "Erro: ocorrência circular em unificação.\n");
                exit(1);
            }
            a->instance = b;
        }
    }
    else if (b->kind == TVAR)
    {
        unify(b, a);
    }
    else if (a->kind == TPRIM && b->kind == TPRIM)
    {
        if (a->prim != b->prim)
        {
            fprintf(stderr, "Erro: tipos primitivos incompatíveis.\n");
            exit(1);
        }
    }
    else if (a->kind == TFUN && b->kind == TFUN)
    {
        unify(a->arg, b->arg);
        unify(a->ret, b->ret);
    }
    else
    {
        fprintf(stderr, "Erro: unificação de tipos incompatíveis.\n");
        exit(1);
    }
}

static void env_add(const char *name, TypeScheme *sch)
{
    EnvEntry *e = malloc(sizeof(EnvEntry));
    size_t len = strlen(name) + 1;
    e->name = malloc(len);
    if (!e->name)
    {
        perror("malloc");
        exit(1);
    }
    memcpy(e->name, name, len);
    e->scheme = sch;
    e->next = env;
    env = e;
}

static TypeScheme *env_lookup(const char *name)
{
    for (EnvEntry *e = env; e; e = e->next)
    {
        if (strcmp(e->name, name) == 0)
            return e->scheme;
    }
    return NULL;
}

static void ftv_type(Type *t, int *seen, int *count)
{
    t = prune(t);
    if (t->kind == TVAR)
    {
        if (!seen[t->var_id])
        {
            seen[t->var_id] = 1;
            (*count)++;
        }
    }
    else if (t->kind == TFUN)
    {
        ftv_type(t->arg, seen, count);
        ftv_type(t->ret, seen, count);
    }
}

static TypeScheme *generalize(Type *t)
{
    int max = next_type_var;
    int *seen = calloc(max, sizeof(int));
    int tot = 0;
    ftv_type(t, seen, &tot);
    int *vars = malloc(tot * sizeof(int));
    int idx = 0;
    for (int i = 0; i < max; i++)
    {
        if (seen[i])
            vars[idx++] = i;
    }
    TypeScheme *sch = malloc(sizeof(TypeScheme));
    sch->var_count = tot;
    sch->vars = vars;
    sch->type = t;
    free(seen);
    return sch;
}

static Type *copy_type(Type *t, Type **map)
{
    t = prune(t);
    if (t->kind == TVAR)
    {
        return map[t->var_id] ? map[t->var_id] : t;
    }
    else if (t->kind == TPRIM)
    {
        return new_prim(t->prim);
    }
    else
    { // TFUN
        Type *a = copy_type(t->arg, map);
        Type *r = copy_type(t->ret, map);
        return new_fun(a, r);
    }
}

static Type *instantiate(TypeScheme *sch)
{
    int max = next_type_var;
    Type **map = calloc(max, sizeof(Type *));
    for (int i = 0; i < sch->var_count; i++)
    {
        map[sch->vars[i]] = new_type_var();
    }
    Type *inst = copy_type(sch->type, map);
    free(map);
    return inst;
}

static Type *infer(ASTNode *node)
{
    switch (node->type)
    {
    case AST_NUMBER:
    {
        Type *res = new_prim(TYPE_NUMBER);
        node->data_type = type_to_datatype(res);
        return res;
    }
    case AST_STRING:
    {
        Type *res = new_prim(TYPE_STRING);
        node->data_type = type_to_datatype(res);
        return res;
    }
    case AST_VARIABLE:
    {
        TypeScheme *sch = env_lookup(node->variable.name);
        if (!sch)
        {
            fprintf(stderr, "Erro: variável '%s' não declarada.\n", node->variable.name);
            exit(1);
        }
        Type *res = instantiate(sch);
        node->data_type = type_to_datatype(res);
        return res;
    }
    case AST_BINARY_OP:
    {
        Type *l = infer(node->binary_op.left);
        Type *r = infer(node->binary_op.right);
        const char *op = node->binary_op.operator;
        if (!strcmp(op, "+") || !strcmp(op, "-") || !strcmp(op, "*") || !strcmp(op, "/"))
        {
            unify(l, new_prim(TYPE_NUMBER));
            unify(r, new_prim(TYPE_NUMBER));
            return new_prim(TYPE_NUMBER);
        }
        else
        {
            unify(l, r);
            return new_prim(TYPE_BOOLEAN);
        }
    }
    case AST_VARIABLE_DECLARATION:
    {
        Type *t = new_type_var();
        if (node->variable_declaration.expression)
        {
            Type *et = infer(node->variable_declaration.expression);
            unify(t, et);
        }
        env_add(node->variable_declaration.name, generalize(t));
        return t;
    }
    case AST_ASSIGNMENT:
    {
        Type *et = infer(node->assignment.expression);
        TypeScheme *sch = env_lookup(node->assignment.variable->variable.name);
        Type *vt = instantiate(sch);
        unify(vt, et);
        return et;
    }
    case AST_BLOCK:
    {
        Type *last = NULL;
        for (int i = 0; i < node->block.statement_count; i++)
            last = infer(node->block.statements[i]);
        return last ? last : new_prim(TYPE_NIL);
    }
    case AST_IF_STATEMENT:
    {
        Type *cond = infer(node->if_statement.condition);
        unify(cond, new_prim(TYPE_BOOLEAN));
        Type *t1 = infer(node->if_statement.then_branch);
        Type *t2 = node->if_statement.else_branch ? infer(node->if_statement.else_branch) : new_prim(TYPE_NIL);
        unify(t1, t2);
        return t1;
    }
    case AST_WHILE_STATEMENT:
        unify(infer(node->while_statement.condition), new_prim(TYPE_BOOLEAN));
        infer(node->while_statement.body);
        return new_prim(TYPE_NIL);
    case AST_FUNCTION_DECLARATION:
    {
        int n = node->function_declaration.param_count;
        Type **params = malloc(n * sizeof(Type *));
        for (int i = 0; i < n; i++)
        {
            params[i] = new_type_var();
            env_add(node->function_declaration.parameters[i]->function_parameter.name, generalize(params[i]));
        }
        Type *body_t = infer(node->function_declaration.body);
        Type *fun_t = body_t;
        for (int i = n - 1; i >= 0; i--)
            fun_t = new_fun(params[i], fun_t);
        env_add(node->function_declaration.name, generalize(fun_t));
        return fun_t;
    }
    case AST_FUNCTION_CALL:
    {
        ASTNode fn_node = {0};
        fn_node.type = AST_VARIABLE;
        strncpy(fn_node.variable.name, node->function_call.function_name, MAX_TOKEN_LEN);
        Type *ft = infer(&fn_node);
        for (int i = 0; i < node->function_call.arg_count; i++)
        {
            Type *arg_t = infer(node->function_call.arguments[i]);
            Type *res = new_type_var();
            unify(ft, new_fun(arg_t, res));
            ft = res;
        }
        return ft;
    }
    default:
        return new_prim(TYPE_UNKNOWN);
    }
}

void semantic_check(ASTNode *root)
{
    env = NULL;
    next_type_var = 0;
    infer(root);
}
