# C++ Interpreter

## Description

This project implements a custom interpreter for a simplified C++ like language.
It supports variables, arrays, control flow, functions, and expression evaluation.

## Features

* Static variable and array declarations (`LET`)
* Arithmetic and logical expressions
* Conditional statements (`IF / ELSE / ENDIF`)
* Loops (`WHILE / DONE`)
* Functions with parameters and return values
* Input/Output (`READ`, `PRINT`)
* Error handling (lexical, syntax, semantic, runtime)

## Architecture

The interpreter is structured in multiple layers:

* **Lexer** – converts input text into tokens
* **Parser** – builds an Abstract Syntax Tree (AST)
* **AST** – represents the program structure
* **Interpreter** – executes the AST (`eval` / `exec`)

## Language Overview

### Variables and Arrays

```
LET x
LET arr[5]
```

### Assignment

```
x = 10
```

### Input / Output

```
READ x
PRINT x
PRINT 10 * 2
```

### Conditionals

```
IF (x > 0)
    PRINT x
ELSE
    PRINT 0
ENDIF
```

### Loops

```
WHILE (x > 0)
    PRINT x
DONE
```

### Functions

```
FUNCTION sum(a, b)
    RETURN a + b
DONE
```

## Data Types

* Single data type: `double`
* `0` → false
* non-zero → true

## Scope

* Static (lexical) scoping
* Nested scopes for blocks and functions
* Symbol tables implemented with `std::unordered_map`

## Error Handling

The interpreter detects:

* Lexical errors (invalid tokens)
* Syntax errors (invalid structure)
* Semantic errors (undeclared variables, wrong arguments)
* Runtime errors (division by zero, invalid array access)

## Example Programs

Example programs can be found in the `examples` folder.

## Technologies

* C++
* Object-Oriented Programming
* Recursive Descent Parsing

