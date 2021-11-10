#!/bin/sh
bison -t -d -v sintactico.y
flex lexico.l
gcc lex.yy.c sintactico.tab.c symtab.c types.c arithmetic_ops.c bool_ops.c -lfl -lm -g -o julia_compiler
