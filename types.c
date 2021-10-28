#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "symtab.h"

void error(char *str) {
    printf("%s", str);
    exit(1);
}

void debug_print(char* str, Variable var) {
        switch (var.type) {
            case Int64:
                printf("[DEBUG INT %s] %s = %i\n", str, var.var_name, var.val.Int64);
                break;
            case Float64:
                printf("[DEBUG FLOAT %s] %s = %f\n", str, var.var_name, var.val.Float64);
                break;
            case String:
                printf("[DEBUG STRING %s] %s = %s\n", str, var.var_name, var.val.String);
                break;
            case Bool:
                printf("[DEBUG BOOL %s] %s = %i\n", str, var.var_name, var.val.Bool);
                break;
            default:
                printf("Unknown type: %i\n", var.type);
                break;
        }
}

void store_val(Variable var, int debug) {
    int ret = sym_add(var.var_name, &var);
    if(ret == SYMTAB_DUPLICATE) {
        if(sym_remove(var.var_name) != SYMTAB_OK) error("Error while removing the var to store new val!");
        ret = sym_add(var.var_name, &var);
    }
    if(ret != SYMTAB_OK) error("Error when storing value in symtab!\n");
    if(debug == 1) debug_print("store_val", var);
}

void show_val(char *key, int debug) {
    Variable v;
    if(sym_lookup(key, &v) != SYMTAB_OK) error("Error in symtab loockup!\n");
    if(debug == 1) debug_print("show_val", v);
}