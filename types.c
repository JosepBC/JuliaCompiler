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

void print_vector(Variable v) {
    printf("[print_vector types.c] ");
    switch (v.type) {
        case Int64Vector:
            for(int i = 0; i < v.val.Int64Vector.n_elem; i++) printf("%i ", v.val.Int64Vector.v[i]);
            printf("\n");
            break;
        case Float64Vector:
            for(int i = 0; i < v.val.Float64Vector.n_elem; i++) printf("%f ", v.val.Float64Vector.v[i]);
            printf("\n");
            break;
        default:
            error("Ilegal type in print_vector!");
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
            case Int64Vector:
                printf("[DEBUG %s] Int64Vector %s = \n", str, var.var_name);
                print_vector(var);
                break;
            case Float64Vector:
                printf("[DEBUG %s] Float64Vector %s = \n", str, var.var_name);
                print_vector(var);
                break;
            case Int64Matrix:
                printf("[DEBUG %s] Int64Matrix %s = \n", str, var.var_name);
                print_matrix(var);
                break;
            case Float64Matrix:
                printf("[DEBUG %s] Float64Matrix %s = \n", str, var.var_name);
                print_matrix(var);
                break;
            default:
                printf("Unknown type in debug_print: %i\n", var.type);
                break;
        }
}

void crop_first_last_elem(char **str) {
    *str = *str + 1;
    (*str)[strlen(*str) - 1] = '\0';
}

void print_matrix(Variable v) {
    switch (v.type) {
        case Int64Matrix:
            for(int i = 0; i < v.val.Int64Matrix.n_rows; i++) {
                for(int j = 0; j < v.val.Int64Matrix.n_cols; j++){
                    printf("%i ", v.val.Int64Matrix.m[i * v.val.Int64Matrix.n_cols + j]);
                }
                printf("\n");
            }
            break;
        case Float64Matrix:
            for(int i = 0; i < v.val.Float64Matrix.n_rows; i++) {
                for(int j = 0; j < v.val.Float64Matrix.n_cols; j++){
                    printf("%f ", v.val.Float64Matrix.m[i * v.val.Float64Matrix.n_cols + j]);
                }
                printf("\n");
            }
            break;
        default:
            error("Ilegal type in print_matrix");
            break;
    }
}

void store_val(Variable var, int debug) {
    if(debug == 1) debug_print("store_val", var);
    int ret = sym_add(var.var_name, &var);
    if(ret == SYMTAB_DUPLICATE) {
        ret = sym_remove(var.var_name);
        if(ret != SYMTAB_OK) symtab_error_handle("removing the var to store new val in store_val!", ret);
        ret = sym_add(var.var_name, &var);
    }
    if(ret != SYMTAB_OK) symtab_error_handle("storing value in symtab in store_val!", ret);
}

void show_val(char *key, int debug) {
    Variable v;
    int ret = sym_lookup(key, &v);
    if(ret != SYMTAB_OK) symtab_error_handle("loockup in show val!", ret);
    if(debug == 1) debug_print("show_val", v);
}

void fill_vector(char *in_str, Variable *var) {
    printf("[DEBUG fill_vector types.c] Vector string: %s\n", in_str);
    int n_elem = 1;
    for(int i = 0; in_str[i] != '\0'; i++) {
        if(in_str[i] == ';') n_elem++;
    }
    printf("[DEBUG fill_vector types.c] Nelem in vector = %i\n", n_elem);

    switch (var->type) {
        case Int64Vector:
            var->val.Int64Vector.n_elem = n_elem;
            var->val.Int64Vector.v = (int *) malloc(sizeof(int) * n_elem);
            char *token = strtok(in_str, ";");
            for(int i = 0; i < n_elem && token != NULL; i++) {
                var->val.Int64Vector.v[i] = atoi(token);
                token = strtok(NULL, ";");
            }
            break;
        case Float64Vector:
            var->val.Float64Vector.n_elem = n_elem;
            var->val.Float64Vector.v = (float *) malloc(sizeof(float) * n_elem);
            char *t = strtok(in_str, ";");
            for(int i = 0; i < n_elem && t != NULL; i++) {
                var->val.Float64Vector.v[i] = atof(t);
                t = strtok(NULL, ";");
            }
            break;
        default:
            error("Ilegal type in vector!");
            break;
    }
}

void print_node_col(NodeCol *col) {
    switch (col->col_type) {
       case Int64:
           while(col != NULL) {
               printf("%i ", col->val.val.Int64);
               col = col->next;
           }
           printf("\n");
           break;
       case Float64:
           while(col != NULL) {
               printf("%f ", col->val.val.Float64);
               col = col->next;
           }
           printf("\n");
           break;
       default:
           error("Ilegal type in matrix!");
           break;
    }

}

void print_node_row(NodeRow *row) {
    printf("Dyn Matrix:\n");
    while(row != NULL) {
        print_node_col(row->val);
        row = row->next;
    }
}

void store_matrix(NodeRow *row, Variable *var) {
    if(row->row_type == Int64) var->type = Int64Matrix;
    else if(row->row_type == Float64) var->type = Float64Matrix;
    else error("Ilegal type in matrix!");
    NodeRow *r = row;
    switch (var->type) {
        case Int64Matrix: {
                var->val.Int64Matrix.m = (int *) malloc(sizeof(int) * row->n_elem_row * row->val->n_elem_col);
                int *ptr = var->val.Int64Matrix.m;
                var->val.Int64Matrix.n_cols = row->val->n_elem_col;
                var->val.Int64Matrix.n_rows = row->n_elem_row;
                while(r != NULL) {
                    NodeCol *c = r->val;
                    while(c != NULL) {
                        *(ptr++) = c->val.val.Int64;
                        c = c->next;
                    }
                    r = r->next;
                }
            }
            break;
        case Float64Matrix: {
                var->val.Float64Matrix.m = (float *) malloc(sizeof(float) * row->n_elem_row * row->val->n_elem_col);
                float *ptr = var->val.Float64Matrix.m;
                var->val.Float64Matrix.n_cols = row->val->n_elem_col;
                var->val.Float64Matrix.n_rows = row->n_elem_row;
                while(r != NULL) {
                    NodeCol *c = r->val;
                    while(c != NULL) {
                        *(ptr++) = c->val.val.Float64;
                        c = c->next;
                    }
                    r = r->next;
                }
            }
            break;
    
    }
}