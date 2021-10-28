#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "symtab.h"

void error(char *str) {
    printf("%s", str);
    exit(1);
}

void symtab_error_handle(const char *str, int error_code) {
    switch (error_code) {
        case SYMTAB_OK:
            break;
        case SYMTAB_NO_MEMORY:
            printf("[ERROR] Symtab no memory error %s\n", str);
            exit(1);
            break;
        case SYMTAB_DUPLICATE:
            printf("[ERROR] Symtab duplicate error in %s\n", str);
            exit(1);
            break;
        case SYMTAB_NOT_FOUND:
            printf("[ERROR] Symtab not found error in %s\n", str);
            exit(1);
            break;
        case SYMTAB_STACK_OVERFLOW:
            printf("[ERROR] Symtab stack overflow in %s\n", str);
            exit(1);
            break;
        case SYMTAB_STACK_UNDERFLOW:
            printf("[ERROR] Symtab stack underflow in %s\n", str);
            exit(1);
            break;
        case SYMTAB_NOT_TOP:
            printf("[ERROR] Symtab not top in %s\n", str);
            exit(1);
            break;
    }
}

void debug_print(char* str, Variable var) {
        switch (var.type) {
            case Int64:
                printf("[DEBUG %s] Int64 %s = %i\n", str, var.var_name, var.val.Int64);
                break;
            case Float64:
                printf("[DEBUG %s] Float64 %s = %f\n", str, var.var_name, var.val.Float64);
                break;
            case String:
                printf("[DEBUG %s] String %s = %s\n", str, var.var_name, var.val.String);
                break;
            case Bool:
                printf("[DEBUG %s] Bool %s = %i\n", str, var.var_name, var.val.Bool);
                break;
            default:
                printf("Unknown type: %i\n", var.type);
                break;
        }
}

void store_val(Variable var, int debug) {
    int ret = sym_add(var.var_name, &var);
    if(ret == SYMTAB_DUPLICATE) {
        ret = sym_remove(var.var_name);
        if(ret != SYMTAB_OK) symtab_error_handle("removing the var to store new val in store_val!", ret);
        ret = sym_add(var.var_name, &var);
    }
    if(ret != SYMTAB_OK) symtab_error_handle("storing value in symtab in store_val!", ret);
    if(debug == 1) debug_print("store_val", var);
}

void show_val(char *key, int debug) {
    Variable v;
    int ret = sym_lookup(key, &v);
    if(ret != SYMTAB_OK) symtab_error_handle("loockup in show val!", ret);
    if(debug == 1) debug_print("show_val", v);
}