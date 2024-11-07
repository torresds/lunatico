#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#define MAX_TOKEN_LEN 256

typedef enum {
    TOKEN_EOF,
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_KEYWORD,
    TOKEN_PAREN_OPEN,
    TOKEN_PAREN_CLOSE,
    TOKEN_BRACE_OPEN,
    TOKEN_BRACE_CLOSE,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
    int line;
    int column;
} Token;

typedef struct {
    FILE *file;
    int current_char;
    int line;
    int column;
    int debug_mode; 
} LexerState;

/**
 * Inicializa o lexer com o arquivo fornecido.
 *
 * @param lexer Ponteiro para o estado do lexer.
 * @param file Arquivo a ser analisado.
 */
void lexer_init(LexerState *lexer, FILE *file);

/**
 * Obtém o próximo token do arquivo.
 *
 * @param lexer Ponteiro para o estado do lexer.
 * @return Token obtido.
 */
Token lexer_get_next_token(LexerState *lexer);

/**
 * Imprime informações sobre o token (para depuração).
 *
 * @param token Token a ser impresso.
 */
void token_print(Token token);/**
 * Inicializa o lexer com o arquivo fornecido.
 *
 * @param lexer Ponteiro para o estado do lexer.
 * @param file Arquivo a ser analisado.
 */
void lexer_init(LexerState *lexer, FILE *file);

/**
 * Obtém o próximo token do arquivo.
 *
 * @param lexer Ponteiro para o estado do lexer.
 * @return Token obtido.
 */
Token lexer_get_next_token(LexerState *lexer);

/**
 * Imprime informações sobre o token (para depuração).
 *
 * @param token Token a ser impresso.
 */
void token_print(Token token);

#endif
