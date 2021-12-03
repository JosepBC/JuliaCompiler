#!/bin/sh
bison -t -d -v -r all -Wall -Werror sintactico.y
flex lexico.l
gcc lex.yy.c sintactico.tab.c symtab.c types.c arithmetic_ops.c code_generation.c -lfl -lm -g -o julia_compiler
