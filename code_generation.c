#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include "code_generation.h"

int tmp_count = 0;

void emet(bool print_file, FILE *out, char *str, ...) {
   va_list args;

   va_start(args, str);
   vprintf(str, args);
   if(print_file) vfprintf(out, str, args);
   va_end(args);
}

void generate_tmp(char *tmp_name) {
   sprintf(tmp_name, "$t%i", tmp_count++);
}
