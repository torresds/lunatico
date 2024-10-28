#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

char *keywords[] = {
    "if", "then", "else", "end", "function", "local", "return", "while", "do", NULL};

// Funções auxiliares (prototipação)
void lexer_advance(LexerState *lexer);
void lexer_skip_whitespace(LexerState *lexer);
void lexer_skip_comment(LexerState *lexer);
int is_identifier_start(int c);
int is_identifier_part(int c);
int is_operator_char(int c);
int is_keyword(const char *str);
int peek_char(LexerState *lexer);

void lexer_init(LexerState *lexer, FILE *file)
{
    lexer->file = file;
    lexer->current_char = fgetc(file);
    lexer->line = 1;
    lexer->column = 1;
}

void lexer_advance(LexerState *lexer)
{
    if (lexer->current_char == '\n')
    {
        lexer->line++;
        lexer->column = 0;
    }
    lexer->current_char = fgetc(lexer->file);
    lexer->column++;
    if (lexer->debug_mode)
    {
        printf("[Lexer] Avançado para o caractere '%c' (Linha %d, Coluna %d)\n",
               lexer->current_char, lexer->line, lexer->column);
    }
}

void lexer_skip_whitespace(LexerState *lexer)
{
    while (isspace(lexer->current_char))
    {
        lexer_advance(lexer);
    }
}

void lexer_skip_comment(LexerState *lexer)
{
    if (lexer->current_char == '-')
    {
        lexer_advance(lexer);
        if (lexer->current_char == '-')
        {
            lexer_advance(lexer);
            if (lexer->current_char == '[')
            {
                lexer_advance(lexer);
                if (lexer->current_char == '[')
                {
                    lexer_advance(lexer);
                    while (lexer->current_char != EOF)
                    {
                        if (lexer->current_char == ']')
                        {
                            lexer_advance(lexer);
                            if (lexer->current_char == ']')
                            {
                                lexer_advance(lexer);
                                break;
                            }
                        }
                        else
                        {
                            lexer_advance(lexer);
                        }
                    }
                }
                else
                {
                    // Comentário de linha
                    while (lexer->current_char != '\n' && lexer->current_char != EOF)
                    {
                        lexer_advance(lexer);
                    }
                }
            }
            else
            {
                // Comentário de linha
                while (lexer->current_char != '\n' && lexer->current_char != EOF)
                {
                    lexer_advance(lexer);
                }
            }
        }
        else
        {
            // Não é um comentário, retorna ao caractere anterior
            ungetc(lexer->current_char, lexer->file);
            lexer->current_char = '-';
            lexer->column--;
        }
    }
}

int is_identifier_start(int c)
{
    return isalpha(c) || c == '_';
}

int is_identifier_part(int c)
{
    return isalnum(c) || c == '_';
}

int is_operator_char(int c)
{
    return strchr("+-*/%<>=~.", c) != NULL;
}

int is_keyword(const char *str)
{
    for (int i = 0; keywords[i] != NULL; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Função para espiar o próximo caractere sem consumir o atual
int peek_char(LexerState *lexer)
{
    int c = fgetc(lexer->file);
    ungetc(c, lexer->file);
    return c;
}

Token lexer_get_next_token(LexerState *lexer)
{
    Token token;
    token.type = TOKEN_UNKNOWN;
    token.value[0] = '\0';
    token.line = lexer->line;
    token.column = lexer->column;
    int idx = 0;
    if (lexer->debug_mode)
    {
        printf("[Lexer] Iniciando lexer_get_next_token\n");
    }
    while (1)
    {
        lexer_skip_whitespace(lexer);
        lexer_skip_comment(lexer);

        if (!isspace(lexer->current_char) && lexer->current_char != '-')
        {
            break;
        }
    }

    if (lexer->current_char == EOF || lexer->current_char == -1)
    {
        token.type = TOKEN_EOF;
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token EOF reconhecido\n");
        }
        return token;
    }

    // Identificadores e palavras-chave
    if (is_identifier_start(lexer->current_char))
    {
        while (is_identifier_part(lexer->current_char) && idx < MAX_TOKEN_LEN - 1)
        {
            token.value[idx++] = lexer->current_char;
            lexer_advance(lexer);
        }
        token.value[idx] = '\0';

        if (is_keyword(token.value))
        {
            token.type = TOKEN_KEYWORD;
        }
        else
        {
            token.type = TOKEN_IDENTIFIER;
        }
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token reconhecido: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   token.type, token.value, token.line, token.column);
        }
        return token;
    }

    // Números (inteiros e decimais)
    if (isdigit(lexer->current_char) || (lexer->current_char == '.' && isdigit(peek_char(lexer))))
    {
        int has_dot = 0;
        if (lexer->current_char == '.')
        {
            has_dot = 1;
            token.value[idx++] = lexer->current_char;
            lexer_advance(lexer);
        }
        while ((isdigit(lexer->current_char) || lexer->current_char == '.') && idx < MAX_TOKEN_LEN - 1)
        {
            if (lexer->current_char == '.')
            {
                if (has_dot)
                {
                    break; // Segundo ponto encontrado, termina número
                }
                has_dot = 1;
            }
            token.value[idx++] = lexer->current_char;
            lexer_advance(lexer);
        }
        token.value[idx] = '\0';
        token.type = TOKEN_NUMBER;
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token reconhecido: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   token.type, token.value, token.line, token.column);
        }
        return token;
    }

    // Strings com escape
    if (lexer->current_char == '"' || lexer->current_char == '\'')
    {
        char quote_type = lexer->current_char;
        lexer_advance(lexer); // Consumir a aspa inicial
        while (lexer->current_char != quote_type && lexer->current_char != EOF && idx < MAX_TOKEN_LEN - 1)
        {
            if (lexer->current_char == '\\')
            { // Escape
                lexer_advance(lexer);
                if (lexer->current_char == EOF)
                {
                    break;
                }
                switch (lexer->current_char)
                {
                case 'n':
                    token.value[idx++] = '\n';
                    break;
                case 't':
                    token.value[idx++] = '\t';
                    break;
                case 'r':
                    token.value[idx++] = '\r';
                    break;
                case '\\':
                    token.value[idx++] = '\\';
                    break;
                case '\'':
                    token.value[idx++] = '\'';
                    break;
                case '\"':
                    token.value[idx++] = '\"';
                    break;
                default:
                    token.value[idx++] = lexer->current_char;
                    break;
                }
            }
            else
            {
                token.value[idx++] = lexer->current_char;
            }
            lexer_advance(lexer);
        }
        lexer_advance(lexer); // Consumir a aspa final
        token.value[idx] = '\0';
        token.type = TOKEN_STRING;
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token reconhecido: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   token.type, token.value, token.line, token.column);
        }
        return token;
    }

    // Operadores compostos
    if (is_operator_char(lexer->current_char))
    {
        token.value[idx++] = lexer->current_char;
        lexer_advance(lexer);

        // Verificar se é um operador composto
        if ((token.value[0] == '<' || token.value[0] == '>' || token.value[0] == '=' || token.value[0] == '~') && lexer->current_char == '=')
        {
            token.value[idx++] = lexer->current_char;
            lexer_advance(lexer);
        }
        else if (token.value[0] == '.' && lexer->current_char == '.')
        {
            token.value[idx++] = lexer->current_char;
            lexer_advance(lexer);
            if (lexer->current_char == '.')
            {
                token.value[idx++] = lexer->current_char;
                lexer_advance(lexer);
            }
        }

        token.value[idx] = '\0';
        token.type = TOKEN_OPERATOR;
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token reconhecido: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   token.type, token.value, token.line, token.column);
        }
        return token;
    }

    // Parênteses e outros símbolos
    switch (lexer->current_char)
    {
    case '(':
        token.type = TOKEN_PAREN_OPEN;
        lexer_advance(lexer);
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token reconhecido: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   token.type, token.value, token.line, token.column);
        }
        return token;
    case ')':
        token.type = TOKEN_PAREN_CLOSE;
        lexer_advance(lexer);
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token reconhecido: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   token.type, token.value, token.line, token.column);
        }
        return token;
    case '{':
        token.type = TOKEN_BRACE_OPEN;
        lexer_advance(lexer);
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token reconhecido: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   token.type, token.value, token.line, token.column);
        }
        return token;
    case '}':
        token.type = TOKEN_BRACE_CLOSE;
        lexer_advance(lexer);
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token reconhecido: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   token.type, token.value, token.line, token.column);
        }
        return token;
    case ';':
        token.type = TOKEN_SEMICOLON;
        lexer_advance(lexer);
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token reconhecido: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   token.type, token.value, token.line, token.column);
        }
        return token;
    case ',':
        token.type = TOKEN_COMMA;
        lexer_advance(lexer);
        if (lexer->debug_mode)
        {
            printf("[Lexer] Token reconhecido: Tipo=%d, Valor='%s', Linha=%d, Coluna=%d\n",
                   token.type, token.value, token.line, token.column);
        }
        return token;
    default:
        fprintf(stderr, "Erro léxico: Caractere desconhecido '%c' na linha %d, coluna %d\n",
                lexer->current_char, lexer->line, lexer->column);
        exit(EXIT_FAILURE);
    }
}

void token_print(Token token)
{
    const char *type_str;
    switch (token.type)
    {
    case TOKEN_EOF:
        type_str = "EOF";
        break;
    case TOKEN_NUMBER:
        type_str = "NUMBER";
        break;
    case TOKEN_IDENTIFIER:
        type_str = "IDENTIFIER";
        break;
    case TOKEN_STRING:
        type_str = "STRING";
        break;
    case TOKEN_OPERATOR:
        type_str = "OPERATOR";
        break;
    case TOKEN_KEYWORD:
        type_str = "KEYWORD";
        break;
    case TOKEN_PAREN_OPEN:
        type_str = "PAREN_OPEN";
        break;
    case TOKEN_PAREN_CLOSE:
        type_str = "PAREN_CLOSE";
        break;
    case TOKEN_BRACE_OPEN:
        type_str = "BRACE_OPEN";
        break;
    case TOKEN_BRACE_CLOSE:
        type_str = "BRACE_CLOSE";
        break;
    case TOKEN_SEMICOLON:
        type_str = "SEMICOLON";
        break;
    case TOKEN_COMMA:
        type_str = "COMMA";
        break;
    default:
        type_str = "UNKNOWN";
        break;
    }
    printf("Token(Type: %s, Value: '%s', Line: %d, Column: %d)\n",
           type_str, token.value, token.line, token.column);
}
