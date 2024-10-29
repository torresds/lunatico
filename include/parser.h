#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "lexer.h"
#include "ast.h"

typedef struct {
    LexerState *lexer;
    Token current_token;
    int debug_mode;
} ParserState;

void parser_init(ParserState *parser, LexerState *lexer);
ASTNode *parse(ParserState *parser);
void parser_eat(ParserState *parser, TokenType type);
Token parser_peek_next_token(ParserState *parser);

#endif
