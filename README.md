# Lunatico 
**Lunatico** é um compilador para a linguagem Lua, escrito em C. Um projeto escrito totalmente como hobby, para aprender mais sobre linguagens.

## Requisitos
- **Compilador C**: GCC ou compatível com o padrão C99.
- **Make**: Para compilar o projeto usando o Makefile.

## Uso
```bash
./lunatico [opções] <arquivo.lua>
```
### Opções
- `--debug`: Ativa o modo de depuração, exibindo mensagens detalhadas durante a análise léxica e sintática.
- `--lexer`: Testa o lexer individualmente, imprimindo todos os tokens identificados no arquivo de entrada.

## Estrutura do Projeto
```makefile
lunatico/
├── include/ # Contém os arquivos header (.h) com declarações e definições compartilhadas.
│   ├── lexer.h 
│   └── parser.h
├── src/
│   ├── lexer.c
│   ├── parser.c
│   └── main.c
├── Makefile
├── teste.lua     # Código Lua de exemplo para teste
└── README.md     # Este arquivo
```

## 🚧 TODO List
- [ ] Análise Semântica
  - [ ] Verificação de Tipos
  - [ ] Resolução de Identificadores e Escopos
- [ ] Geração de código
  - [ ] Geração de Código Intermediário
  - [ ] Geração de Código de Máquina
