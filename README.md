# B-Minor Lexer

This project is a **lexical analyzer (lexer)** for the **B-Minor programming language**.  
It scans input source files, identifies valid tokens, and detects lexical errors.

## About B-Minor
B-Minor is a **small, C-like programming language** designed for teaching compiler construction.  
More details can be found in the **official documentation**:  
📖 [B-Minor Language Specification](https://www3.nd.edu/~dthain/compilerbook/chapterB.pdf)

## Features
- Tokenizes **identifiers, keywords, numbers, operators, delimiters, and strings**.
- Detects **invalid tokens** and reports errors with their exact location.
- Ignores **comments** (`//` and `/* ... */`).
- Handles **symbol table management** for identifiers.
- Implements **line and column tracking** for accurate error reporting.
