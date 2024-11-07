#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *keywords[] = {
    "if", "then", "else", "end", "function", "return", "while", "do", "local", NULL
};

void lexer_init(LexerState *lexer, FILE *file) {
    lexer->file = file;
    lexer->current_char = fgetc(file);
    lexer->line = 1;
    lexer->column = 1;
    lexer->debug_mode = 0; 
}

void lexer_advance(LexerState *lexer) {
    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 0;
    }
    lexer->current_char = fgetc(lexer->file);
    lexer->column++;
}

void lexer_skip_whitespace(LexerState *lexer) {
    while (isspace(lexer->current_char)) {
        lexer_advance(lexer);
    }
}

void lexer_skip_comment(LexerState *lexer) {
    if (lexer->current_char == '-') {
        lexer_advance(lexer);
        if (lexer->current_char == '-') {
            lexer_advance(lexer);
            if (lexer->current_char == '[') {
                lexer_advance(lexer);
                if (lexer->current_char == '[') {
                    lexer_advance(lexer);
                    while (lexer->current_char != EOF) {
                        if (lexer->current_char == ']') {
                            lexer_advance(lexer);
                            if (lexer->current_char == ']') {
                                lexer_advance(lexer);
                                break;
                            }
                        } else {
                            lexer_advance(lexer);
                        }
                    }
                } else {
                    while (lexer->current_char != '\n' && lexer->current_char != EOF) {
                        lexer_advance(lexer);
                    }
                }
            } else {
                while (lexer->current_char != '\n' && lexer->current_char != EOF) {
                    lexer_advance(lexer);
                }
            }
        } else {
            ungetc(lexer->current_char, lexer->file);
            lexer->current_char = '-';
            lexer->column--;
        }
    }
}

int is_keyword(const char *str) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(str, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

Token lexer_get_next_token(LexerState *lexer) {
    Token token;
    token.line = lexer->line;
    token.column = lexer->column;
    if (lexer->debug_mode) {
        printf("[Lexer] Iniciando lexer_get_next_token\n");
    }
    while (1) {
        lexer_skip_whitespace(lexer);
        lexer_skip_comment(lexer);

        if (!isspace(lexer->current_char) && lexer->current_char != '-') {
            break;
        }
    }

    if (lexer->current_char == EOF) {
        token.type = TOKEN_EOF;
        strcpy(token.value, "EOF");
        return token;
    }

    if (isalpha(lexer->current_char) || lexer->current_char == '_') {
        int length = 0;
        char buffer[MAX_TOKEN_LEN] = {0};
        while (isalnum(lexer->current_char) || lexer->current_char == '_') {
            if (length < MAX_TOKEN_LEN - 1) {
                buffer[length++] = lexer->current_char;
            }
            lexer_advance(lexer);
        }
        buffer[length] = '\0';

        token.type = is_keyword(buffer) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
        strcpy(token.value, buffer);
        return token;
    }

    if (isdigit(lexer->current_char)) {
        int length = 0;
        char buffer[MAX_TOKEN_LEN] = {0};
        while (isdigit(lexer->current_char) || lexer->current_char == '.') {
            if (length < MAX_TOKEN_LEN - 1) {
                buffer[length++] = lexer->current_char;
            }
            lexer_advance(lexer);
        }
        buffer[length] = '\0';
        token.type = TOKEN_NUMBER;
        strcpy(token.value, buffer);
        return token;
    }

    if (lexer->current_char == '"') {
        lexer_advance(lexer);
        int length = 0;
        char buffer[MAX_TOKEN_LEN] = {0};
        while (lexer->current_char != '"' && lexer->current_char != EOF) {
            if (length < MAX_TOKEN_LEN - 1) {
                buffer[length++] = lexer->current_char;
            }
            lexer_advance(lexer);
        }
        if (lexer->current_char == '"') {
            lexer_advance(lexer);
        } else {
            fprintf(stderr, "Erro léxico: String não terminada na linha %d, coluna %d\n", lexer->line, lexer->column);
            exit(EXIT_FAILURE);
        }
        buffer[length] = '\0';
        token.type = TOKEN_STRING;
        strcpy(token.value, buffer);
        return token;
    }

    switch (lexer->current_char) {
        case '+': case '-': case '*': case '/': case '%':
        case '=': case '~': case '<': case '>': {
            int length = 0;
            char buffer[3] = {0};
            buffer[length++] = lexer->current_char;
            lexer_advance(lexer);

            if ((buffer[0] == '=' && lexer->current_char == '=') ||
                (buffer[0] == '~' && lexer->current_char == '=') ||
                (buffer[0] == '<' && lexer->current_char == '=') ||
                (buffer[0] == '>' && lexer->current_char == '=')) {
                buffer[length++] = lexer->current_char;
                lexer_advance(lexer);
            }

            buffer[length] = '\0';
            token.type = TOKEN_OPERATOR;
            strcpy(token.value, buffer);
            return token;
        }
        case '(':
            token.type = TOKEN_PAREN_OPEN;
            strcpy(token.value, "(");
            lexer_advance(lexer);
            return token;
        case ')':
            token.type = TOKEN_PAREN_CLOSE;
            strcpy(token.value, ")");
            lexer_advance(lexer);
            return token;
        case ',':
            token.type = TOKEN_COMMA;
            strcpy(token.value, ",");
            lexer_advance(lexer);
            return token;
        case ';':
            token.type = TOKEN_SEMICOLON;
            strcpy(token.value, ";");
            lexer_advance(lexer);
            return token;
        case ':':
            token.type = TOKEN_COLON;
            strcpy(token.value, ":");
            lexer_advance(lexer);
            return token;
        default:
            fprintf(stderr, "Erro léxico: Caractere desconhecido '%c' na linha %d, coluna %d\n",
                    lexer->current_char, lexer->line, lexer->column);
            exit(EXIT_FAILURE);
    }
}

void token_print(Token token) {
    const char *token_type_names[] = {
        "TOKEN_EOF",
        "TOKEN_IDENTIFIER",
        "TOKEN_NUMBER",
        "TOKEN_STRING",
        "TOKEN_OPERATOR",
        "TOKEN_KEYWORD",
        "TOKEN_PAREN_OPEN",
        "TOKEN_PAREN_CLOSE",
        "TOKEN_COMMA",
        "TOKEN_SEMICOLON",
    };

    printf("Token Type: %s, Value: '%s', Line: %d, Column: %d\n",
           token_type_names[token.type], token.value, token.line, token.column);
}
