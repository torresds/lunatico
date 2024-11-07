#ifndef AST_H
#define AST_H

#include "types.h"

#define MAX_TOKEN_LEN 256

typedef enum {
    AST_NUMBER,
    AST_STRING,
    AST_VARIABLE,
    AST_BINARY_OP,
    AST_ASSIGNMENT,
    AST_IF_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_FUNCTION_CALL,
    AST_FUNCTION_DECLARATION,
    AST_FUNCTION_PARAMETER,
    AST_RETURN_STATEMENT,
    AST_BLOCK,
    AST_VARIABLE_DECLARATION
} ASTNodeType;

typedef struct ASTNode ASTNode;

typedef struct {
    char value[MAX_TOKEN_LEN];
} NumberNode;

typedef struct {
    char value[MAX_TOKEN_LEN];
} StringNode;

typedef struct {
    char name[MAX_TOKEN_LEN];
} VariableNode;

typedef struct {
    char operator[MAX_TOKEN_LEN];
    ASTNode *left;
    ASTNode *right;
} BinaryOpNode;

typedef struct {
    ASTNode *variable;
    ASTNode *expression;
} AssignmentNode;

typedef struct {
    ASTNode *condition;
    ASTNode *then_branch;
    ASTNode *else_branch;
} IfStatementNode;

typedef struct {
    ASTNode *condition;
    ASTNode *body;
} WhileStatementNode;

typedef struct {
    char function_name[MAX_TOKEN_LEN];
    ASTNode **arguments;
    int arg_count;
} FunctionCallNode;

typedef struct {
    char name[MAX_TOKEN_LEN];
    ASTNode **parameters;
    int param_count;
    ASTNode *body;
} FunctionDeclarationNode;

typedef struct {
    char name[MAX_TOKEN_LEN];
} FunctionParameterNode;

typedef struct {
    ASTNode *expression;
} ReturnStatementNode;

typedef struct {
    ASTNode **statements;
    int statement_count;
} BlockNode;

typedef struct {
    char name[MAX_TOKEN_LEN];
    char type_name[MAX_TOKEN_LEN];
    ASTNode *expression;
} VariableDeclarationNode;

struct ASTNode {
    ASTNodeType type;
    DataType data_type;
    union {
        NumberNode number;
        StringNode string;
        VariableNode variable;
        BinaryOpNode binary_op;
        AssignmentNode assignment;
        IfStatementNode if_statement;
        WhileStatementNode while_statement;
        FunctionCallNode function_call;
        FunctionDeclarationNode function_declaration;
        FunctionParameterNode function_parameter;
        ReturnStatementNode return_statement;
        BlockNode block;
        VariableDeclarationNode variable_declaration;
    };
};

ASTNode *create_number_node(const char *value);
ASTNode *create_string_node(const char *value);
ASTNode *create_variable_node(const char *name);
ASTNode *create_binary_op_node(const char *operator, ASTNode *left, ASTNode *right);
ASTNode *create_assignment_node(ASTNode *variable, ASTNode *expression);
ASTNode *create_if_statement_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);
ASTNode *create_while_statement_node(ASTNode *condition, ASTNode *body);
ASTNode *create_function_call_node(const char *function_name, ASTNode **arguments, int arg_count);
ASTNode *create_function_declaration_node(const char *name, ASTNode **parameters, int param_count, ASTNode *body);
ASTNode *create_function_parameter_node(const char *name);
ASTNode *create_return_statement_node(ASTNode *expression);
ASTNode *create_block_node();
ASTNode *create_variable_declaration_node(const char *name, const char *type_name, ASTNode *expression);

void print_ast(ASTNode *node, int indent);
void free_ast(ASTNode *node);

#endif
