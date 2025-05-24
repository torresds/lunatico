#include "semantic.h"
#include "lexer.h"
#include "parser.h"
#include <string.h>

int main(int argc, char *argv[]) {
    int debug_mode = 0;
    int test_lexer = 0;
    char *filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            debug_mode = 1;
        } else if (strcmp(argv[i], "--lexer") == 0) {
            test_lexer = 1;
        } else {
            filename = argv[i];
        }
    }

    if (filename == NULL) {
        printf("Uso: %s [--debug] [--lexer] <arquivo.lua>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    LexerState lexer;
    lexer_init(&lexer, file);
    lexer.debug_mode = debug_mode;

    if (test_lexer) {
        Token token;
        do {
            token = lexer_get_next_token(&lexer);
            token_print(token);
        } while (token.type != TOKEN_EOF);
    } else {
        ParserState parser;
        parser_init(&parser, &lexer);
        parser.debug_mode = debug_mode;

        ASTNode *ast = parse(&parser);

        print_ast(ast, 0);
        semantic_check(ast);
        print_ast(ast, 0);
        printf("Análise semântica concluída com sucesso.\n");
        free_ast(ast);
    }

    fclose(file);
    return 0;
}
