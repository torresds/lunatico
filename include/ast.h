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
} ASTNodeType;

typedef struct ASTNode ASTNode;


// Nó de número
typedef struct {
    char value[MAX_TOKEN_LEN];  // Valor numérico como string
} NumberNode;

// Nó de string
typedef struct {
    char value[MAX_TOKEN_LEN];  // Valor da string
} StringNode;

// Nó de variável
typedef struct {
    char name[MAX_TOKEN_LEN];   // Nome da variável
} VariableNode;

// Nó de operação binária
typedef struct {
    char operator[MAX_TOKEN_LEN];   // Operador ('+', '-', '*', '/', ...)
    ASTNode *left;                  
    ASTNode *right;              
} BinaryOpNode;

// Nó de atribuição
typedef struct {
    ASTNode *variable;      // Variável sendo atribuída
    ASTNode *expression;    // Expressão sendo atribuída à variável
} AssignmentNode;

// Nó de declaração 'if'
typedef struct {
    ASTNode *condition;     // Condição do 'if'
    ASTNode *then_branch;   // Bloco executado se a condição for verdadeira
    ASTNode *else_branch;   // Bloco executado se a condição for falsa (opcional)
} IfStatementNode;

// Nó de declaração 'while'
typedef struct {
    ASTNode *condition;     // Condição do 'while'
    ASTNode *body;          // Corpo do loop
} WhileStatementNode;

// Nó de chamada de função
typedef struct {
    char function_name[MAX_TOKEN_LEN];  // Nome da função
    ASTNode **arguments;                // Lista de argumentos
    int arg_count;                      // Número de argumentos
} FunctionCallNode;

// Nó de declaração de função
typedef struct {
    char name[MAX_TOKEN_LEN];       // Nome da função
    ASTNode **parameters;           // Lista de parâmetros
    int param_count;                // Número de parâmetros
    ASTNode *body;                  // Corpo da função
} FunctionDeclarationNode;

// Nó de parâmetro de função
typedef struct {
    char name[MAX_TOKEN_LEN];       // Nome do parâmetro
} FunctionParameterNode;

// Nó de declaração 'return'
typedef struct {
    ASTNode *expression;            // Expressão a ser retornada (pode ser NULL)
} ReturnStatementNode;

// Nó de bloco
typedef struct {
    ASTNode **statements;           // Lista de declarações
    int statement_count;            // Número de declarações
} BlockNode;

// Estrutura principal do nó da AST
struct ASTNode {
    ASTNodeType type;   // Tipo do nó
    DataType data_type; // Tipo de dados associado ao nó (para análise semântica)
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
        // Outros tipos de nós podem ser adicionados aqui
    };
};

/**
 * Cria um nó de número na AST.
 * @param value O valor numérico representado como string.
 * @return Ponteiro para o nó de número criado.
 */
ASTNode *create_number_node(const char *value);

/**
 * Cria um nó de string na AST.
 * @param value O valor da string.
 * @return Ponteiro para o nó de string criado.
 */
ASTNode *create_string_node(const char *value);

// Cria um nó de variável
ASTNode *create_variable_node(const char *name);

/**
 * Cria um nó de operação binária na AST.
 * @param operator O operador binário (+, -, *, etc.).
 * @param left Ponteiro para o nó esquerdo da operação.
 * @param right Ponteiro para o nó direito da operação.
 * @return Ponteiro para o nó de operação binária criado.
 */
ASTNode *create_binary_op_node(const char *operator, ASTNode *left, ASTNode *right);

// Cria um nó de atribuição
ASTNode *create_assignment_node(ASTNode *variable, ASTNode *expression);

// Cria um nó de declaração 'if'
ASTNode *create_if_statement_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);

// Cria um nó de declaração 'while'
ASTNode *create_while_statement_node(ASTNode *condition, ASTNode *body);

// Cria um nó de chamada de função
ASTNode *create_function_call_node(const char *function_name, ASTNode **arguments, int arg_count);

// Cria um nó de declaração de função
ASTNode *create_function_declaration_node(const char *name, ASTNode **parameters, int param_count, ASTNode *body);

// Cria um nó de parâmetro de função
ASTNode *create_function_parameter_node(const char *name);

// Cria um nó de declaração 'return'
ASTNode *create_return_statement_node(ASTNode *expression);

// Cria um nó de bloco
ASTNode *create_block_node();

// Funções para manipular a AST

// Imprime a AST para fins de depuração
void print_ast(ASTNode *node, int indent);

// Libera a memória alocada para a AST
void free_ast(ASTNode *node);

#endif // AST_H
