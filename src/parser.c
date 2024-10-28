#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Implementações das funções para criar nós da AST

ASTNode *create_number_node(const char *value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_NUMBER;
    strncpy(node->number.value, value, MAX_TOKEN_LEN);
    return node;
}

ASTNode *create_string_node(const char *value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_STRING;
    strncpy(node->string.value, value, MAX_TOKEN_LEN);
    return node;
}

ASTNode *create_variable_node(const char *name)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_VARIABLE;
    strncpy(node->variable.name, name, MAX_TOKEN_LEN);
    return node;
}

ASTNode *create_binary_op_node(const char *operator, ASTNode * left, ASTNode *right)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_BINARY_OP;
    strncpy(node->binary_op.operator, operator, MAX_TOKEN_LEN);
    node->binary_op.left = left;
    node->binary_op.right = right;
    return node;
}

ASTNode *create_assignment_node(ASTNode *variable, ASTNode *expression)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_ASSIGNMENT;
    node->assignment.variable = variable;
    node->assignment.expression = expression;
    return node;
}

ASTNode *create_if_statement_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_IF_STATEMENT;
    node->if_statement.condition = condition;
    node->if_statement.then_branch = then_branch;
    node->if_statement.else_branch = else_branch;
    return node;
}

ASTNode *create_while_statement_node(ASTNode *condition, ASTNode *body)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_WHILE_STATEMENT;
    node->while_statement.condition = condition;
    node->while_statement.body = body;
    return node;
}

ASTNode *create_block_node()
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_BLOCK;
    node->block.statements = NULL;
    node->block.statement_count = 0;
    return node;
}

ASTNode *create_function_call_node(const char *function_name, ASTNode **arguments, int arg_count)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    node->type = AST_FUNCTION_CALL;
    strncpy(node->function_call.function_name, function_name, MAX_TOKEN_LEN);
    node->function_call.arguments = arguments;
    node->function_call.arg_count = arg_count;
    return node;
}


void parser_init(ParserState *parser, LexerState *lexer)
{
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->debug_mode = 0; // Inicializa o modo de depuração como desativado
}

void parser_eat(ParserState *parser, TokenType type)
{
    if (parser->current_token.type == type)
    {
        if (parser->debug_mode)
        {
            printf("[Parser] Consumindo token: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   parser->current_token.type, parser->current_token.value,
                   parser->current_token.line, parser->current_token.column);
        }
        parser->current_token = lexer_get_next_token(parser->lexer);
    }
    else
    {
        fprintf(stderr, "Erro de sintaxe: Esperado token %d, encontrado %d na linha %d, coluna %d\n",
                type, parser->current_token.type, parser->current_token.line, parser->current_token.column);
        exit(EXIT_FAILURE);
    }
}

Token parser_peek_next_token(ParserState *parser)
{
    LexerState *lexer = parser->lexer;

    // Salva o estado atual do lexer
    long pos = ftell(lexer->file);
    int current_char = lexer->current_char;
    int line = lexer->line;
    int column = lexer->column;

    // Obtém o próximo token
    Token next_token = lexer_get_next_token(lexer);

    // Restaura o estado do lexer
    fseek(lexer->file, pos, SEEK_SET);
    lexer->current_char = current_char;
    lexer->line = line;
    lexer->column = column;

    return next_token;
}

ASTNode *parse_expression(ParserState *parser);
ASTNode *parse_term(ParserState *parser);
ASTNode *parse_factor(ParserState *parser);
ASTNode *parse_statement(ParserState *parser);
ASTNode *parse_block(ParserState *parser);
ASTNode *parse_function_call(ParserState *parser);
ASTNode *parse_while_statement(ParserState *parser);
ASTNode *parse_function_call_statement(ParserState *parser);

ASTNode *parse_function_call_statement(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_function_call_statement\n");
    }

    ASTNode *call_node = parse_function_call(parser);

    if (parser->debug_mode) {
        printf("[Parser] Saindo de parse_function_call_statement\n");
    }

    return call_node;
}


ASTNode *parse_while_statement(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_while_statement\n");
    }

    parser_eat(parser, TOKEN_KEYWORD); // 'while'

    ASTNode *condition = parse_expression(parser);

    parser_eat(parser, TOKEN_KEYWORD); // 'do'

    ASTNode *body = parse_block(parser);

    parser_eat(parser, TOKEN_KEYWORD); // 'end'

    if (parser->debug_mode) {
        printf("[Parser] Saindo de parse_while_statement\n");
    }

    return create_while_statement_node(condition, body);
}

ASTNode *parse_factor(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_factor com token '%s'\n", parser->current_token.value);
    }

    Token token = parser->current_token;

    if (token.type == TOKEN_NUMBER) {
        parser_eat(parser, TOKEN_NUMBER);
        ASTNode *node = create_number_node(token.value);
        if (parser->debug_mode) {
            printf("[Parser] Número reconhecido: %s\n", token.value);
        }
        return node;
    } else if (token.type == TOKEN_STRING) {
        parser_eat(parser, TOKEN_STRING);
        ASTNode *node = create_string_node(token.value);
        if (parser->debug_mode) {
            printf("[Parser] String reconhecida: \"%s\"\n", token.value);
        }
        return node;
    } else if (token.type == TOKEN_IDENTIFIER) {
        Token lookahead = parser_peek_next_token(parser);
        if (lookahead.type == TOKEN_PAREN_OPEN) {
            if (parser->debug_mode) {
                printf("[Parser] Reconhecida chamada de função: %s\n", token.value);
            }
            return parse_function_call(parser);
        } else {
            parser_eat(parser, TOKEN_IDENTIFIER);
            ASTNode *node = create_variable_node(token.value);
            if (parser->debug_mode) {
                printf("[Parser] Variável reconhecida: %s\n", token.value);
            }
            return node;
        }
    } else if (token.type == TOKEN_PAREN_OPEN) {
        parser_eat(parser, TOKEN_PAREN_OPEN);
        ASTNode *node = parse_expression(parser);
        parser_eat(parser, TOKEN_PAREN_CLOSE);
        return node;
    } else {
        fprintf(stderr, "Erro de sintaxe: Token inesperado '%s' na linha %d, coluna %d\n",
                token.value, token.line, token.column);
        exit(EXIT_FAILURE);
    }
}

ASTNode *parse_term(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_term\n");
    }

    ASTNode *node = parse_factor(parser);

    while (parser->current_token.type == TOKEN_OPERATOR &&
           (strcmp(parser->current_token.value, "*") == 0 ||
            strcmp(parser->current_token.value, "/") == 0 ||
            strcmp(parser->current_token.value, "%") == 0)) {
        char operator[MAX_TOKEN_LEN];
        strncpy(operator, parser->current_token.value, MAX_TOKEN_LEN);
        parser_eat(parser, TOKEN_OPERATOR);
        if (parser->debug_mode) {
            printf("[Parser] Operador reconhecido em parse_term: %s\n", operator);
        }
        ASTNode *right = parse_factor(parser);
        node = create_binary_op_node(operator, node, right);
    }

    if (parser->debug_mode) {
        printf("[Parser] Saindo de parse_term\n");
    }

    return node;
}

ASTNode *parse_arithmetic_expression(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_arithmetic_expression\n");
    }

    ASTNode *node = parse_term(parser);

    while (parser->current_token.type == TOKEN_OPERATOR &&
           (strcmp(parser->current_token.value, "+") == 0 ||
            strcmp(parser->current_token.value, "-") == 0)) {
        char operator[MAX_TOKEN_LEN];
        strncpy(operator, parser->current_token.value, MAX_TOKEN_LEN);
        parser_eat(parser, TOKEN_OPERATOR);
        if (parser->debug_mode) {
            printf("[Parser] Operador reconhecido em parse_arithmetic_expression: %s\n", operator);
        }
        ASTNode *right = parse_term(parser);
        node = create_binary_op_node(operator, node, right);
    }

    if (parser->debug_mode) {
        printf("[Parser] Saindo de parse_arithmetic_expression\n");
    }

    return node;
}

ASTNode *parse_relational_expression(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_relational_expression\n");
    }

    ASTNode *node = parse_arithmetic_expression(parser);

    while (parser->current_token.type == TOKEN_OPERATOR &&
           (strcmp(parser->current_token.value, "==") == 0 ||
            strcmp(parser->current_token.value, "~=") == 0 ||
            strcmp(parser->current_token.value, "<") == 0 ||
            strcmp(parser->current_token.value, ">") == 0 ||
            strcmp(parser->current_token.value, "<=") == 0 ||
            strcmp(parser->current_token.value, ">=") == 0)) {
        char operator[MAX_TOKEN_LEN];
        strncpy(operator, parser->current_token.value, MAX_TOKEN_LEN);
        parser_eat(parser, TOKEN_OPERATOR);
        if (parser->debug_mode) {
            printf("[Parser] Operador relacional reconhecido: %s\n", operator);
        }
        ASTNode *right = parse_arithmetic_expression(parser);
        node = create_binary_op_node(operator, node, right);
    }

    if (parser->debug_mode) {
        printf("[Parser] Saindo de parse_relational_expression\n");
    }

    return node;
}

ASTNode *parse_expression(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_expression\n");
    }

    ASTNode *node = parse_relational_expression(parser);

    if (parser->debug_mode) {
        printf("[Parser] Saindo de parse_expression\n");
    }

    return node;
}

ASTNode *parse_assignment(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_assignment\n");
    }

    Token var_token = parser->current_token;
    parser_eat(parser, TOKEN_IDENTIFIER);
    ASTNode *var_node = create_variable_node(var_token.value);

    parser_eat(parser, TOKEN_OPERATOR); // Espera '='

    ASTNode *expr_node = parse_expression(parser);

    if (parser->debug_mode) {
        printf("[Parser] Saindo de parse_assignment\n");
    }

    return create_assignment_node(var_node, expr_node);
}

ASTNode *parse_if_statement(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_if_statement\n");
    }

    parser_eat(parser, TOKEN_KEYWORD); // 'if'

    ASTNode *condition = parse_expression(parser);

    parser_eat(parser, TOKEN_KEYWORD); // 'then'

    ASTNode *then_branch = parse_block(parser);

    ASTNode *else_branch = NULL;
    if (parser->current_token.type == TOKEN_KEYWORD &&
        strcmp(parser->current_token.value, "else") == 0) {
        parser_eat(parser, TOKEN_KEYWORD); // 'else'
        else_branch = parse_block(parser);
    }

    parser_eat(parser, TOKEN_KEYWORD); // 'end'

    if (parser->debug_mode) {
        printf("[Parser] Saindo de parse_if_statement\n");
    }

    return create_if_statement_node(condition, then_branch, else_branch);
}

ASTNode *parse_function_call(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_function_call com token '%s'\n", parser->current_token.value);
    }

    Token token = parser->current_token;
    parser_eat(parser, TOKEN_IDENTIFIER);

    parser_eat(parser, TOKEN_PAREN_OPEN);

    // Lista de argumentos
    ASTNode **arguments = NULL;
    int arg_count = 0;

    // Verifica se há argumentos
    if (parser->current_token.type != TOKEN_PAREN_CLOSE) {
        do {
            ASTNode *arg = parse_expression(parser);
            arg_count++;
            arguments = realloc(arguments, arg_count * sizeof(ASTNode *));
            if (!arguments) {
                perror("Erro de alocação de memória");
                exit(EXIT_FAILURE);
            }
            arguments[arg_count - 1] = arg;

            if (parser->current_token.type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        } while (1);
    }

    parser_eat(parser, TOKEN_PAREN_CLOSE);

    if (parser->debug_mode) {
        printf("[Parser] Saindo de parse_function_call\n");
    }

    return create_function_call_node(token.value, arguments, arg_count);
}

ASTNode *parse_statement(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_statement com token '%s'\n", parser->current_token.value);
    }

    if (parser->current_token.type == TOKEN_KEYWORD) {
        if (strcmp(parser->current_token.value, "if") == 0) {
            return parse_if_statement(parser);
        } else if (strcmp(parser->current_token.value, "while") == 0) {
            return parse_while_statement(parser);
        } else {
            fprintf(stderr, "Erro de sintaxe: Palavra-chave inesperada '%s' na linha %d, coluna %d\n",
                    parser->current_token.value, parser->current_token.line, parser->current_token.column);
            exit(EXIT_FAILURE);
        }
    } else if (parser->current_token.type == TOKEN_IDENTIFIER) {
        // Pode ser uma atribuição ou uma chamada de função
        Token lookahead = parser_peek_next_token(parser);
        if (lookahead.type == TOKEN_OPERATOR && strcmp(lookahead.value, "=") == 0) {
            return parse_assignment(parser);
        } else if (lookahead.type == TOKEN_PAREN_OPEN) {
            return parse_function_call_statement(parser);
        } else {
            fprintf(stderr, "Erro de sintaxe: Declaração inválida iniciada com '%s' na linha %d, coluna %d\n",
                    parser->current_token.value, parser->current_token.line, parser->current_token.column);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Erro de sintaxe: Token inesperado '%s' na linha %d, coluna %d\n",
                parser->current_token.value, parser->current_token.line, parser->current_token.column);
        exit(EXIT_FAILURE);
    }
}

ASTNode *parse_block(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Entrando em parse_block\n");
    }

    ASTNode *block = create_block_node();

    while (parser->current_token.type != TOKEN_EOF &&
           !(parser->current_token.type == TOKEN_KEYWORD &&
             (strcmp(parser->current_token.value, "end") == 0 ||
              strcmp(parser->current_token.value, "else") == 0))) {
        ASTNode *statement = parse_statement(parser);

        // Adicionar a declaração ao bloco
        block->block.statement_count++;
        block->block.statements = realloc(block->block.statements, block->block.statement_count * sizeof(ASTNode *));
        if (!block->block.statements) {
            perror("Erro de alocação de memória");
            exit(EXIT_FAILURE);
        }
        block->block.statements[block->block.statement_count - 1] = statement;

        // Verificar se há um ponto e vírgula opcional
        if (parser->current_token.type == TOKEN_SEMICOLON) {
            parser_eat(parser, TOKEN_SEMICOLON);
        }
    }

    if (parser->debug_mode) {
        printf("[Parser] Saindo de parse_block\n");
    }

    return block;
}

ASTNode *parse(ParserState *parser) {
    if (parser->debug_mode) {
        printf("[Parser] Iniciando análise sintática\n");
    }

    ASTNode *ast = parse_block(parser);

    if (parser->debug_mode) {
        printf("[Parser] Análise sintática concluída\n");
    }

    return ast;
}

// Funções para imprimir e liberar a AST

void print_ast(ASTNode *node, int indent)
{
    if (!node)
        return;

    for (int i = 0; i < indent; i++)
        printf("  ");

    switch (node->type)
    {
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
        printf("%*sCondition:\n", indent * 2, "");
        print_ast(node->if_statement.condition, indent + 2);
        printf("%*sThen:\n", indent * 2, "");
        print_ast(node->if_statement.then_branch, indent + 2);
        if (node->if_statement.else_branch)
        {
            printf("%*sElse:\n", indent * 2, "");
            print_ast(node->if_statement.else_branch, indent + 2);
        }
        break;
    case AST_WHILE_STATEMENT:
        printf("WhileStatement\n");
        printf("%*sCondition:\n", indent * 2, "");
        print_ast(node->while_statement.condition, indent + 2);
        printf("%*sBody:\n", indent * 2, "");
        print_ast(node->while_statement.body, indent + 2);
        break;
    case AST_BLOCK:
        printf("Block\n");
        for (int i = 0; i < node->block.statement_count; i++)
        {
            print_ast(node->block.statements[i], indent + 1);
        }
        break;
    case AST_FUNCTION_CALL:
        printf("FunctionCall(%s)\n", node->function_call.function_name);
        for (int i = 0; i < node->function_call.arg_count; i++)
        {
            print_ast(node->function_call.arguments[i], indent + 1);
        }
        break;
    default:
        printf("Unknown node type\n");
    }
}

void free_ast(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
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
        {
            free_ast(node->if_statement.else_branch);
        }
        break;
    case AST_WHILE_STATEMENT:
        free_ast(node->while_statement.condition);
        free_ast(node->while_statement.body);
        break;
    case AST_BLOCK:
        for (int i = 0; i < node->block.statement_count; i++)
        {
            free_ast(node->block.statements[i]);
        }
        free(node->block.statements);
        break;
    case AST_FUNCTION_CALL:
        for (int i = 0; i < node->function_call.arg_count; i++)
        {
            free_ast(node->function_call.arguments[i]);
        }
        free(node->function_call.arguments);
        break;
    default:
        // Para AST_NUMBER, AST_STRING e AST_VARIABLE não há memória adicional a liberar
        break;
    }

    free(node);
}
