#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "symbol_table.h"
#include "ast.h"

unsigned int symbol_hash(const char *name, int size) {
    unsigned int hash = 0;
    for (; *name; name++) {
        hash = (hash << 2) + *name;
    }
    return hash % size;
}


Symbol *symbol_table_lookup(SymbolTable *table, const char *name) {
    unsigned int index = symbol_hash(name, table->size);
    Symbol *symbol = table->symbols[index];
    while (symbol) {
        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
        symbol = symbol->next;
    }
    if (table->parent) {
        return symbol_table_lookup(table->parent, name);
    }
    return NULL;
}


void symbol_table_insert(SymbolTable *table, const char *name, DataType type) {
    unsigned int index = symbol_hash(name, table->size);
    Symbol *symbol = malloc(sizeof(Symbol));
    if (!symbol) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    strncpy(symbol->name, name, MAX_TOKEN_LEN);
    symbol->type = type;
    symbol->next = table->symbols[index];
    table->symbols[index] = symbol;
}


SymbolTable *symbol_table_create(SymbolTable *parent) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (!table) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    table->size = 211; 
    table->symbols = calloc(table->size, sizeof(Symbol *));
    if (!table->symbols) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }
    table->parent = parent;
    return table;
}




SymbolTable *symbol_table_enter_scope(SymbolTable *parent) {
    return symbol_table_create(parent);
}

void symbol_table_exit_scope(SymbolTable *table) {
    for (int i = 0; i < table->size; i++) {
        Symbol *symbol = table->symbols[i];
        while (symbol) {
            Symbol *temp = symbol;
            symbol = symbol->next;
            free(temp);
        }
    }
    free(table->symbols);
    free(table);
}

