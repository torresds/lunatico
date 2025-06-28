# 🌙 Luna

**Luna** is a lightweight, statically-typed programming language with type inference, built entirely in C. Inspired by the simplicity and syntax of Lua, Luna introduces a type-safe, minimalist model with modern programming constructs and explicit semantics.

## 🎯 Goal
The project is built as a personal exploration of compiler construction and programming language design. It is not intended as a production tool, but as a learning environment to prototype features, understand type systems, and build a language from the ground up.

## 💡Philosophy

- **Statically typed**: All expressions are type-checked at compile time.
- **Type inference**: Less boilerplate, smarter typing.
- **Lua-inspired syntax**: Familiar, clean, and easy to read.
- **Safe scoping**: Lexical scope with declaration-before-use enforcement.
- **Functional-friendly**: Functions are first-class values with inferred signatures.

## 📄 Example code

```lua
local x: number = 10

function add(a: number, b: number)
    return a + b
end

local result = add(x, 5)
````

## 📦 Requirements

* C compiler (GCC or compatible, C99 standard)
* `make`

## ⚙️ Building

```bash
make
```

## 🚀 Running

```bash
./luna [options] <file.luna>
```

### Options

* `--debug` → Enables verbose output (lexer/parser traces)
* `--lexer` → Tokenizes input and prints all tokens

## 🗂 Project Structure

```
luna/
├── include/        # Header files (AST, Lexer, Parser, Types, Semantics)
├── src/            # Compiler source files
├── teste.luna      # Sample Luna program
├── Makefile        # Build system
└── README.md       # This file
```

## 🚧 Roadmap

### ✅ Done

* [x] Lexer and tokenizer
* [x] Recursive-descent parser
* [x] Abstract Syntax Tree (AST)
* [x] Type inference and semantic analysis
* [x] Scoped environments with shadowing
* [x] AST pretty-printing with types

### 🧪 Soon

* [ ] Intermediate Representation (IR)
* [ ] Stack-based Virtual Machine (VM)
* [ ] Code generation backend
