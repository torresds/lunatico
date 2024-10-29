#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "types.h"

#define MAX_SYMBOL_NAME_LEN 64

typedef struct Symbol {
    char name[MAX_SYMBOL_NAME_LEN];
    DataType type;
    struct Symbol *next; 
} Symbol;

typedef struct SymbolTable {
    Symbol **symbols;
    int size;
    struct SymbolTable *parent; 
} SymbolTable;

SymbolTable *create_symbol_table(int size, SymbolTable *parent);
void free_symbol_table(SymbolTable *table);

void symbol_table_add(SymbolTable *table, const char *name, DataType type);
Symbol *symbol_table_lookup(SymbolTable *table, const char *name);

#endif 
