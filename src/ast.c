#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Cria um nó de número
ASTNode *create_number_node(const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_NUMBER;
    node->data_type = TYPE_NUMBER;
    strncpy(node->number.value, value, MAX_TOKEN_LEN);
    return node;
}

// Cria um nó de string
ASTNode *create_string_node(const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_STRING;
    node->data_type = TYPE_STRING;
    strncpy(node->string.value, value, MAX_TOKEN_LEN);
    return node;
}

// Cria um nó de variável
ASTNode *create_variable_node(const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_VARIABLE;
    node->data_type = TYPE_UNKNOWN;
    strncpy(node->variable.name, name, MAX_TOKEN_LEN);
    return node;
}

// Cria um nó de operação binária
ASTNode *create_binary_op_node(const char *operator, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_BINARY_OP;
    node->data_type = TYPE_UNKNOWN; // Tipo será determinado durante a análise semântica
    strncpy(node->binary_op.operator, operator, MAX_TOKEN_LEN);
    node->binary_op.left = left;
    node->binary_op.right = right;
    return node;
}

// Cria um nó de atribuição
ASTNode *create_assignment_node(ASTNode *variable, ASTNode *expression) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_ASSIGNMENT;
    node->data_type = TYPE_UNKNOWN;
    node->assignment.variable = variable;
    node->assignment.expression = expression;
    return node;
}

// Cria um nó de declaração 'if'
ASTNode *create_if_statement_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_IF_STATEMENT;
    node->data_type = TYPE_UNKNOWN;
    node->if_statement.condition = condition;
    node->if_statement.then_branch = then_branch;
    node->if_statement.else_branch = else_branch;
    return node;
}

// Cria um nó de declaração 'while'
ASTNode *create_while_statement_node(ASTNode *condition, ASTNode *body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_WHILE_STATEMENT;
    node->data_type = TYPE_UNKNOWN;
    node->while_statement.condition = condition;
    node->while_statement.body = body;
    return node;
}

// Cria um nó de chamada de função
ASTNode *create_function_call_node(const char *function_name, ASTNode **arguments, int arg_count) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_FUNCTION_CALL;
    node->data_type = TYPE_UNKNOWN;
    strncpy(node->function_call.function_name, function_name, MAX_TOKEN_LEN);
    node->function_call.arguments = arguments;
    node->function_call.arg_count = arg_count;
    return node;
}

// Cria um nó de declaração de função
ASTNode *create_function_declaration_node(const char *name, ASTNode **parameters, int param_count, ASTNode *body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_FUNCTION_DECLARATION;
    node->data_type = TYPE_FUNCTION;
    strncpy(node->function_declaration.name, name, MAX_TOKEN_LEN);
    node->function_declaration.parameters = parameters;
    node->function_declaration.param_count = param_count;
    node->function_declaration.body = body;
    return node;
}

// Cria um nó de parâmetro de função
ASTNode *create_function_parameter_node(const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_FUNCTION_PARAMETER;
    node->data_type = TYPE_UNKNOWN;
    strncpy(node->function_parameter.name, name, MAX_TOKEN_LEN);
    return node;
}

// Cria um nó de declaração 'return'
ASTNode *create_return_statement_node(ASTNode *expression) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_RETURN_STATEMENT;
    node->data_type = TYPE_UNKNOWN;
    node->return_statement.expression = expression;
    return node;
}

// Cria um nó de bloco
ASTNode *create_block_node() {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_BLOCK;
    node->data_type = TYPE_UNKNOWN;
    node->block.statements = NULL;
    node->block.statement_count = 0;
    return node;
}

// Função para imprimir a AST 
void print_ast(ASTNode *node, int indent) {
    if (!node)
        return;

    for (int i = 0; i < indent; i++)
        printf("  ");

    switch (node->type) {
        case AST_NUMBER:
            printf("Number(%s)\n", node->number.value);
            break;
        case AST_STRING:
            printf("String(\"%s\")\n", node->string.value);
            break;
        case AST_VARIABLE:
            printf("Variable(%s)\n", node->variable.name);
            break;
        case AST_BINARY_OP:
            printf("BinaryOp(%s)\n", node->binary_op.operator);
            print_ast(node->binary_op.left, indent + 1);
            print_ast(node->binary_op.right, indent + 1);
            break;
        case AST_ASSIGNMENT:
            printf("Assignment\n");
            print_ast(node->assignment.variable, indent + 1);
            print_ast(node->assignment.expression, indent + 1);
            break;
        case AST_IF_STATEMENT:
            printf("IfStatement\n");
            printf("%*sCondition:\n", (indent + 1) * 2, "");
            print_ast(node->if_statement.condition, indent + 2);
            printf("%*sThen:\n", (indent + 1) * 2, "");
            print_ast(node->if_statement.then_branch, indent + 2);
            if (node->if_statement.else_branch) {
                printf("%*sElse:\n", (indent + 1) * 2, "");
                print_ast(node->if_statement.else_branch, indent + 2);
            }
            break;
        case AST_WHILE_STATEMENT:
            printf("WhileStatement\n");
            printf("%*sCondition:\n", (indent + 1) * 2, "");
            print_ast(node->while_statement.condition, indent + 2);
            printf("%*sBody:\n", (indent + 1) * 2, "");
            print_ast(node->while_statement.body, indent + 2);
            break;
        case AST_FUNCTION_CALL:
            printf("FunctionCall(%s)\n", node->function_call.function_name);
            for (int i = 0; i < node->function_call.arg_count; i++) {
                print_ast(node->function_call.arguments[i], indent + 1);
            }
            break;
        case AST_FUNCTION_DECLARATION:
            printf("FunctionDeclaration(%s)\n", node->function_declaration.name);
            printf("%*sParameters:\n", (indent + 1) * 2, "");
            for (int i = 0; i < node->function_declaration.param_count; i++) {
                print_ast(node->function_declaration.parameters[i], indent + 2);
            }
            printf("%*sBody:\n", (indent + 1) * 2, "");
            print_ast(node->function_declaration.body, indent + 2);
            break;
        case AST_FUNCTION_PARAMETER:
            printf("Parameter(%s)\n", node->function_parameter.name);
            break;
        case AST_RETURN_STATEMENT:
            printf("ReturnStatement\n");
            if (node->return_statement.expression) {
                print_ast(node->return_statement.expression, indent + 1);
            }
            break;
        case AST_BLOCK:
            printf("Block\n");
            for (int i = 0; i < node->block.statement_count; i++) {
                print_ast(node->block.statements[i], indent + 1);
            }
            break;
        default:
            printf("Unknown node type\n");
            break;
    }
}

// Função para liberar a memória da AST
void free_ast(ASTNode *node) {
    if (!node)
        return;

    switch (node->type) {
        case AST_BINARY_OP:
            free_ast(node->binary_op.left);
            free_ast(node->binary_op.right);
            break;
        case AST_ASSIGNMENT:
            free_ast(node->assignment.variable);
            free_ast(node->assignment.expression);
            break;
        case AST_IF_STATEMENT:
            free_ast(node->if_statement.condition);
            free_ast(node->if_statement.then_branch);
            if (node->if_statement.else_branch)
                free_ast(node->if_statement.else_branch);
            break;
        case AST_WHILE_STATEMENT:
            free_ast(node->while_statement.condition);
            free_ast(node->while_statement.body);
            break;
        case AST_FUNCTION_CALL:
            for (int i = 0; i < node->function_call.arg_count; i++) {
                free_ast(node->function_call.arguments[i]);
            }
            free(node->function_call.arguments);
            break;
        case AST_FUNCTION_DECLARATION:
            for (int i = 0; i < node->function_declaration.param_count; i++) {
                free_ast(node->function_declaration.parameters[i]);
            }
            free(node->function_declaration.parameters);
            free_ast(node->function_declaration.body);
            break;
        case AST_RETURN_STATEMENT:
            if (node->return_statement.expression)
                free_ast(node->return_statement.expression);
            break;
        case AST_BLOCK:
            for (int i = 0; i < node->block.statement_count; i++) {
                free_ast(node->block.statements[i]);
            }
            free(node->block.statements);
            break;
        default:
            break;
    }

    free(node);
}
