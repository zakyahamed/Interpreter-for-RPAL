# RPAL Interpreter
This repository contains a lexical analyser and parser for the RPAL language.

### Team members:
- Kopimenon
- Zaky Ahamed

### Problem Statement

The project requirement involves implementing a lexical analyzer and parser for the RPAL language. The parser's output should be an Abstract Syntax Tree (AST) corresponding to the given input program. Additionally, you need to implement an algorithm to convert the AST into a Standardized Tree (ST) and develop a CSE machine. The program must be capable of reading an input file containing an RPAL program, and the output should align with the results produced by "rpal.exe" for the corresponding program.

### Program Execution Instructions

The following sequence of commands can be used in the root of the project directory
to compile the program and execute rpal programs:
```
> make
> ./rpal20 file_name
```
To generate the Abstract Syntax Tree:
```
> ./rpal20 -ast file_name
```
To generate the Standardized Tree:
```
> ./rpal20 -st file_name
```

### Structure of the Project

This project was coded entirely in C++. It consists of mainly 5 files. They are,
1. main.cpp
2. parser.h
3. tree.h
4. token.h
5. environment.h
