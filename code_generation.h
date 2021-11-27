#ifndef CODE_GENERATION_HEADER
#define CODE_GENERATION_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

extern void emet(bool print_file, FILE *out, char *str, ...);
extern void generate_tmp(char *tmp_name);

#endif