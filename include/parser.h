#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    AST_NUMBER,
    AST_STRING,
    AST_VARIABLE,
    AST_BINARY_OP,
    AST_ASSIGNMENT,
    AST_IF_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_BLOCK,
    AST_FUNCTION_CALL
} ASTNodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeType type;
    union {
        // números
        struct {
            char value[MAX_TOKEN_LEN];
        } number;

        // strings
        struct {
            char value[MAX_TOKEN_LEN];
        } string;

        // variáveis
        struct {
            char name[MAX_TOKEN_LEN];
        } variable;

        // operações binárias
        struct {
            char operator[MAX_TOKEN_LEN];
            ASTNode *left;
            ASTNode *right;
        } binary_op;

        // atribuições
        struct {
            ASTNode *variable;
            ASTNode *expression;
        } assignment;

        // declarações if
        struct {
            ASTNode *condition;
            ASTNode *then_branch;
            ASTNode *else_branch;
        } if_statement;

        // loops while
        struct {
            ASTNode *condition;
            ASTNode *body;
        } while_statement;

        // blocos de código
        struct {
            ASTNode **statements;
            int statement_count;
        } block;

        // chamadas de função
        struct {
            char function_name[MAX_TOKEN_LEN];
            ASTNode **arguments;
            int arg_count;
        } function_call;
    };
};

typedef struct {
    LexerState *lexer;
    Token current_token;
    int debug_mode; 
} ParserState;


/**
 * Cria um nó AST para um número.
 *
 * @param value Valor do número.
 * @return Ponteiro para o nó AST criado.
 */
ASTNode *create_number_node(const char *value);

/**
 * Cria um nó AST para uma string.
 *
 * @param value Valor da string.
 * @return Ponteiro para o nó AST criado.
 */
ASTNode *create_string_node(const char *value);

/**
 * Cria um nó AST para uma variável.
 *
 * @param name Nome da variável.
 * @return Ponteiro para o nó AST criado.
 */
ASTNode *create_variable_node(const char *name);

/**
 * Cria um nó AST para uma operação binária.
 *
 * @param operator Operador.
 * @param left Operando esquerdo.
 * @param right Operando direito.
 * @return Ponteiro para o nó AST criado.
 */
ASTNode *create_binary_op_node(const char *operator, ASTNode *left, ASTNode *right);

/**
 * Cria um nó AST para uma atribuição.
 *
 * @param variable Variável sendo atribuída.
 * @param expression Expressão atribuída.
 * @return Ponteiro para o nó AST criado.
 */
ASTNode *create_assignment_node(ASTNode *variable, ASTNode *expression);

/**
 * Cria um nó AST para uma declaração if.
 *
 * @param condition Condição do if.
 * @param then_branch Bloco then.
 * @param else_branch Bloco else (pode ser NULL).
 * @return Ponteiro para o nó AST criado.
 */
ASTNode *create_if_statement_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);

/**
 * Cria um nó AST para um loop while.
 *
 * @param condition Condição do while.
 * @param body Corpo do loop.
 * @return Ponteiro para o nó AST criado.
 */
ASTNode *create_while_statement_node(ASTNode *condition, ASTNode *body);

/**
 * Cria um nó AST para um bloco de código.
 *
 * @return Ponteiro para o nó AST criado.
 */
ASTNode *create_block_node();

/**
 * Cria um nó AST para uma chamada de função.
 *
 * @param function_name Nome da função.
 * @param arguments Lista de argumentos (pode ser NULL).
 * @param arg_count Número de argumentos.
 * @return Ponteiro para o nó AST criado.
 */
ASTNode *create_function_call_node(const char *function_name, ASTNode **arguments, int arg_count);

// Funções do parser

/**
 * Inicializa o parser com o lexer fornecido.
 *
 * @param parser Ponteiro para o estado do parser.
 * @param lexer Ponteiro para o estado do lexer.
 */
void parser_init(ParserState *parser, LexerState *lexer);

/**
 * Inicia o processo de análise sintática.
 *
 * @param parser Ponteiro para o estado do parser.
 * @return Ponteiro para a raiz da AST.
 */
ASTNode *parse(ParserState *parser);

/**
 * Imprime a AST para fins de depuração.
 *
 * @param node Ponteiro para o nó da AST.
 * @param indent Nível de indentação.
 */
void print_ast(ASTNode *node, int indent);

/**
 * Libera a memória alocada para a AST.
 *
 * @param node Ponteiro para o nó da AST a ser liberado.
 */
void free_ast(ASTNode *node);

#endif 
