#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inicializa o estado do parser
void parser_init(ParserState *parser, LexerState *lexer)
{
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->debug_mode = 0; // Modo de depuração desativado por padrão
}

// Consome o token atual se corresponder ao tipo esperado
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
        fprintf(stderr, "Erro de sintaxe: Esperado token %d, encontrado %d ('%s') na linha %d, coluna %d\n",
                type, parser->current_token.type, parser->current_token.value,
                parser->current_token.line, parser->current_token.column);
        exit(EXIT_FAILURE);
    }
}

// Olha o próximo token sem consumi-lo
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

// Prototipação das funções do parser
ASTNode *parse_expression(ParserState *parser);
ASTNode *parse_term(ParserState *parser);
ASTNode *parse_factor(ParserState *parser);
ASTNode *parse_statement(ParserState *parser);
ASTNode *parse_block(ParserState *parser);
ASTNode *parse_function_call(ParserState *parser);
ASTNode *parse_while_statement(ParserState *parser);
ASTNode *parse_function_call_statement(ParserState *parser);
ASTNode *parse_if_statement(ParserState *parser);
ASTNode *parse_assignment(ParserState *parser);
ASTNode *parse_function_declaration(ParserState *parser);
ASTNode *parse_return_statement(ParserState *parser);

// Implementações das funções de parsing

// Parsea uma chamada de função como uma declaração
ASTNode *parse_function_call_statement(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_function_call_statement\n");
    }

    ASTNode *call_node = parse_function_call(parser);

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_function_call_statement\n");
    }

    return call_node;
}

// Parsea uma declaração 'while'
ASTNode *parse_while_statement(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_while_statement\n");
    }

    parser_eat(parser, TOKEN_KEYWORD); // 'while'

    ASTNode *condition = parse_expression(parser);

    parser_eat(parser, TOKEN_KEYWORD); // 'do'

    ASTNode *body = parse_block(parser);

    parser_eat(parser, TOKEN_KEYWORD); // 'end'

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_while_statement\n");
    }

    return create_while_statement_node(condition, body);
}

// Parsea um fator (número, string, variável, chamada de função ou expressão entre parênteses)
ASTNode *parse_factor(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_factor com token '%s'\n", parser->current_token.value);
    }

    Token token = parser->current_token;

    if (token.type == TOKEN_NUMBER)
    {
        parser_eat(parser, TOKEN_NUMBER);
        ASTNode *node = create_number_node(token.value);
        if (parser->debug_mode)
        {
            printf("[Parser] Número reconhecido: %s\n", token.value);
        }
        return node;
    }
    else if (token.type == TOKEN_STRING)
    {
        parser_eat(parser, TOKEN_STRING);
        ASTNode *node = create_string_node(token.value);
        if (parser->debug_mode)
        {
            printf("[Parser] String reconhecida: \"%s\"\n", token.value);
        }
        return node;
    }
    else if (token.type == TOKEN_IDENTIFIER)
    {
        Token lookahead = parser_peek_next_token(parser);
        if (lookahead.type == TOKEN_PAREN_OPEN)
        {
            if (parser->debug_mode)
            {
                printf("[Parser] Reconhecida chamada de função: %s\n", token.value);
            }
            return parse_function_call(parser);
        }
        else
        {
            parser_eat(parser, TOKEN_IDENTIFIER);
            ASTNode *node = create_variable_node(token.value);
            if (parser->debug_mode)
            {
                printf("[Parser] Variável reconhecida: %s\n", token.value);
            }
            return node;
        }
    }
    else if (token.type == TOKEN_PAREN_OPEN)
    {
        parser_eat(parser, TOKEN_PAREN_OPEN);
        ASTNode *node = parse_expression(parser);
        parser_eat(parser, TOKEN_PAREN_CLOSE);
        return node;
    }
    else
    {
        fprintf(stderr, "Erro de sintaxe: Token inesperado '%s' na linha %d, coluna %d\n",
                token.value, token.line, token.column);
        exit(EXIT_FAILURE);
    }
}

// Parsea um termo (fatores separados por '*' ou '/')
ASTNode *parse_term(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_term\n");
    }

    ASTNode *node = parse_factor(parser);

    while (parser->current_token.type == TOKEN_OPERATOR &&
           (strcmp(parser->current_token.value, "*") == 0 ||
            strcmp(parser->current_token.value, "/") == 0 ||
            strcmp(parser->current_token.value, "%") == 0))
    {
        char operator[MAX_TOKEN_LEN];
        strncpy(operator, parser->current_token.value, MAX_TOKEN_LEN);
        parser_eat(parser, TOKEN_OPERATOR);
        if (parser->debug_mode)
        {
            printf("[Parser] Operador reconhecido em parse_term: %s\n", operator);
        }
        ASTNode *right = parse_factor(parser);
        node = create_binary_op_node(operator, node, right);
    }

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_term\n");
    }

    return node;
}

// Parsea uma expressão aritmética (termos separados por '+' ou '-')
ASTNode *parse_arithmetic_expression(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_arithmetic_expression\n");
    }

    ASTNode *node = parse_term(parser);

    while (parser->current_token.type == TOKEN_OPERATOR &&
           (strcmp(parser->current_token.value, "+") == 0 ||
            strcmp(parser->current_token.value, "-") == 0))
    {
        char operator[MAX_TOKEN_LEN];
        strncpy(operator, parser->current_token.value, MAX_TOKEN_LEN);
        parser_eat(parser, TOKEN_OPERATOR);
        if (parser->debug_mode)
        {
            printf("[Parser] Operador reconhecido em parse_arithmetic_expression: %s\n", operator);
        }
        ASTNode *right = parse_term(parser);
        node = create_binary_op_node(operator, node, right);
    }

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_arithmetic_expression\n");
    }

    return node;
}

// Parsea uma expressão relacional (expressões aritméticas separadas por operadores relacionais)
ASTNode *parse_relational_expression(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_relational_expression\n");
    }

    ASTNode *node = parse_arithmetic_expression(parser);

    while (parser->current_token.type == TOKEN_OPERATOR &&
           (strcmp(parser->current_token.value, "==") == 0 ||
            strcmp(parser->current_token.value, "~=") == 0 ||
            strcmp(parser->current_token.value, "<") == 0 ||
            strcmp(parser->current_token.value, ">") == 0 ||
            strcmp(parser->current_token.value, "<=") == 0 ||
            strcmp(parser->current_token.value, ">=") == 0))
    {
        char operator[MAX_TOKEN_LEN];
        strncpy(operator, parser->current_token.value, MAX_TOKEN_LEN);
        parser_eat(parser, TOKEN_OPERATOR);
        if (parser->debug_mode)
        {
            printf("[Parser] Operador relacional reconhecido: %s\n", operator);
        }
        ASTNode *right = parse_arithmetic_expression(parser);
        node = create_binary_op_node(operator, node, right);
    }

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_relational_expression\n");
    }

    return node;
}

// Parsea uma expressão geral
ASTNode *parse_expression(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_expression\n");
    }

    ASTNode *node = parse_relational_expression(parser);

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_expression\n");
    }

    return node;
}

// Parsea uma atribuição
ASTNode *parse_assignment(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_assignment\n");
    }

    Token var_token = parser->current_token;
    parser_eat(parser, TOKEN_IDENTIFIER);
    ASTNode *var_node = create_variable_node(var_token.value);

    parser_eat(parser, TOKEN_OPERATOR); // Espera '='

    ASTNode *expr_node = parse_expression(parser);

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_assignment\n");
    }

    return create_assignment_node(var_node, expr_node);
}

// Parsea uma declaração 'if'
ASTNode *parse_if_statement(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_if_statement\n");
    }

    parser_eat(parser, TOKEN_KEYWORD); // 'if'

    ASTNode *condition = parse_expression(parser);

    parser_eat(parser, TOKEN_KEYWORD); // 'then'

    ASTNode *then_branch = parse_block(parser);

    ASTNode *else_branch = NULL;
    if (parser->current_token.type == TOKEN_KEYWORD &&
        strcmp(parser->current_token.value, "else") == 0)
    {
        parser_eat(parser, TOKEN_KEYWORD); // 'else'
        else_branch = parse_block(parser);
    }

    parser_eat(parser, TOKEN_KEYWORD); // 'end'

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_if_statement\n");
    }

    return create_if_statement_node(condition, then_branch, else_branch);
}

// Parsea uma chamada de função
ASTNode *parse_function_call(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_function_call com token '%s'\n", parser->current_token.value);
    }

    Token token = parser->current_token;
    parser_eat(parser, TOKEN_IDENTIFIER);

    parser_eat(parser, TOKEN_PAREN_OPEN);

    // Lista de argumentos
    ASTNode **arguments = NULL;
    int arg_count = 0;

    // Verifica se há argumentos
    if (parser->current_token.type != TOKEN_PAREN_CLOSE)
    {
        do
        {
            ASTNode *arg = parse_expression(parser);
            arg_count++;
            arguments = realloc(arguments, arg_count * sizeof(ASTNode *));
            if (!arguments)
            {
                perror("Erro de alocação de memória");
                exit(EXIT_FAILURE);
            }
            arguments[arg_count - 1] = arg;

            if (parser->current_token.type == TOKEN_COMMA)
            {
                parser_eat(parser, TOKEN_COMMA);
            }
            else
            {
                break;
            }
        } while (1);
    }

    parser_eat(parser, TOKEN_PAREN_CLOSE);

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_function_call\n");
    }

    return create_function_call_node(token.value, arguments, arg_count);
}

// Parsea uma declaração de função
ASTNode *parse_function_declaration(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_function_declaration\n");
    }

    parser_eat(parser, TOKEN_KEYWORD); // 'function'

    char function_name[MAX_TOKEN_LEN] = {0};
    if (parser->current_token.type == TOKEN_IDENTIFIER)
    {
        strncpy(function_name, parser->current_token.value, MAX_TOKEN_LEN);
        parser_eat(parser, TOKEN_IDENTIFIER);
    }
    else
    {
        fprintf(stderr, "Erro de sintaxe: Esperado nome da função na linha %d, coluna %d\n",
                parser->current_token.line, parser->current_token.column);
        exit(EXIT_FAILURE);
    }

    parser_eat(parser, TOKEN_PAREN_OPEN);

    // Lista de parâmetros
    ASTNode **parameters = NULL;
    int param_count = 0;

    if (parser->current_token.type != TOKEN_PAREN_CLOSE)
    {
        do
        {
            if (parser->current_token.type == TOKEN_IDENTIFIER)
            {
                ASTNode *param_node = create_function_parameter_node(parser->current_token.value);
                parser_eat(parser, TOKEN_IDENTIFIER);

                param_count++;
                parameters = realloc(parameters, param_count * sizeof(ASTNode *));
                if (!parameters)
                {
                    perror("Erro de alocação de memória");
                    exit(EXIT_FAILURE);
                }
                parameters[param_count - 1] = param_node;

                if (parser->current_token.type == TOKEN_COMMA)
                {
                    parser_eat(parser, TOKEN_COMMA);
                }
                else
                {
                    break;
                }
            }
            else
            {
                fprintf(stderr, "Erro de sintaxe: Esperado nome do parâmetro na linha %d, coluna %d\n",
                        parser->current_token.line, parser->current_token.column);
                exit(EXIT_FAILURE);
            }
        } while (1);
    }

    parser_eat(parser, TOKEN_PAREN_CLOSE);

    // Corpo da função
    ASTNode *body = parse_block(parser);

    parser_eat(parser, TOKEN_KEYWORD); // 'end'

    ASTNode *node = create_function_declaration_node(function_name, parameters, param_count, body);

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_function_declaration\n");
    }

    return node;
}

ASTNode *parse_return_statement(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_return_statement\n");
    }

    parser_eat(parser, TOKEN_KEYWORD); // 'return'

    ASTNode *expr = NULL;
    if (parser->current_token.type != TOKEN_SEMICOLON &&
        parser->current_token.type != TOKEN_EOF &&
        !(parser->current_token.type == TOKEN_KEYWORD && strcmp(parser->current_token.value, "end") == 0))
    {
        expr = parse_expression(parser);
    }

    ASTNode *node = create_return_statement_node(expr);

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_return_statement\n");
    }

    return node;
}

ASTNode *parse_statement(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_statement com token '%s'\n", parser->current_token.value);
    }

    if (parser->current_token.type == TOKEN_KEYWORD)
    {
        if (strcmp(parser->current_token.value, "if") == 0)
        {
            return parse_if_statement(parser);
        }
        else if (strcmp(parser->current_token.value, "while") == 0)
        {
            return parse_while_statement(parser);
        }
        else if (strcmp(parser->current_token.value, "function") == 0)
        {
            return parse_function_declaration(parser);
        }
        else if (strcmp(parser->current_token.value, "return") == 0)
        {
            return parse_return_statement(parser);
        }
        else
        {
            fprintf(stderr, "Erro de sintaxe: Palavra-chave inesperada '%s' na linha %d, coluna %d\n",
                    parser->current_token.value, parser->current_token.line, parser->current_token.column);
            exit(EXIT_FAILURE);
        }
    }
    else if (parser->current_token.type == TOKEN_IDENTIFIER)
    {
        Token lookahead = parser_peek_next_token(parser);
        if (lookahead.type == TOKEN_OPERATOR && strcmp(lookahead.value, "=") == 0)
        {
            return parse_assignment(parser);
        }
        else if (lookahead.type == TOKEN_PAREN_OPEN)
        {
            return parse_function_call_statement(parser);
        }
        else
        {
            fprintf(stderr, "Erro de sintaxe: Declaração inválida iniciada com '%s' na linha %d, coluna %d\n",
                    parser->current_token.value, parser->current_token.line, parser->current_token.column);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Erro de sintaxe: Token inesperado '%s' na linha %d, coluna %d\n",
                parser->current_token.value, parser->current_token.line, parser->current_token.column);
        exit(EXIT_FAILURE);
    }
}

ASTNode *parse_block(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Entrando em parse_block\n");
    }

    ASTNode *block = create_block_node();

    while (parser->current_token.type != TOKEN_EOF &&
           !(parser->current_token.type == TOKEN_KEYWORD &&
             (strcmp(parser->current_token.value, "end") == 0 ||
              strcmp(parser->current_token.value, "else") == 0 ||
              strcmp(parser->current_token.value, "elseif") == 0)))
    {
        ASTNode *statement = parse_statement(parser);

        block->block.statement_count++;
        block->block.statements = realloc(block->block.statements, block->block.statement_count * sizeof(ASTNode *));
        if (!block->block.statements)
        {
            perror("Erro de alocação de memória");
            exit(EXIT_FAILURE);
        }
        block->block.statements[block->block.statement_count - 1] = statement;

        if (parser->current_token.type == TOKEN_SEMICOLON)
        {
            parser_eat(parser, TOKEN_SEMICOLON);
        }
    }

    if (parser->debug_mode)
    {
        printf("[Parser] Saindo de parse_block\n");
    }

    return block;
}

ASTNode *parse(ParserState *parser)
{
    if (parser->debug_mode)
    {
        printf("[Parser] Iniciando análise sintática\n");
    }

    ASTNode *ast = parse_block(parser);

    if (parser->debug_mode)
    {
        printf("[Parser] Análise sintática concluída\n");
    }

    return ast;
}
