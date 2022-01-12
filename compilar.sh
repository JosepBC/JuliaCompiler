#!/bin/sh
if [ ! -d "build" ]
then
    mkdir build
fi

bison -t -d -v -r all -Wall -Werror src/sintactico.y -o build/sintactico.tab.c
flex -o build/lex.yy.c src/lexico.l
gcc build/lex.yy.c  build/sintactico.tab.c src/symtab.c src/types.c src/arithmetic_ops.c src/code_generation.c src/bool_ops.c -I include/ -lfl -lm -g -o julia_compiler
