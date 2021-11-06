#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "types.h"
#include "symtab.h"

void error(char *str) {
    printf("%s\n", str);
    exit(1);
}

bool is_int_vector(Variable v) {return v.type == Int64Vector;}

bool is_float_vector(Variable v) {return v.type == Float64Vector;}

bool is_int_matrix(Variable v) {return v.type == Int64Matrix;}

bool is_float_matrix(Variable v) {return v.type == Float64Matrix;}

bool is_int_or_float(Variable v1) {
    return is_int(v1) || is_float(v1);
}

bool is_int(Variable v) {
    return v.type == Int64;
}

bool is_float(Variable v) {
    return v.type == Float64;
}

bool is_matrix(Variable v) {
    return v.type == Int64Matrix || v.type == Float64Matrix;
}

bool is_vector(Variable v) {
    return is_int_vector(v) || is_float_vector(v);
}

void do_pow(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_int(v2)) {
        res->type = Int64;
        res->val.Int64 = pow(v1.val.Int64, v2.val.Int64);
        return;
    }

    res->type = Float64;

    if(is_int(v1) && is_float(v2)) {
        res->val.Float64 = pow(v1.val.Int64, v2.val.Float64);
    }

    if(is_float(v1) && is_int(v2)) {
        res->val.Float64 = pow(v1.val.Float64, v2.val.Int64);
    }

    if(is_float(v1) && is_float(v2)) {
        res->val.Float64 = pow(v1.val.Float64, v2.val.Float64);
    }

}

void do_div(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_int(v2)) {
        res->type = Int64;
        res->val.Int64 = v1.val.Int64 / v2.val.Int64;
        return;
    } 
    
    res->type = Float64;
    
    if(is_int(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Int64 / v2.val.Float64;
    }

    if(is_float(v1) && is_int(v2)) {
        res->val.Float64 = v1.val.Float64 / v2.val.Int64;
    }

    if(is_float(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Float64 / v2.val.Float64;
    }

}

void mult_int_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Int64Vector.v[i] = v1.val.Int64Vector.v[i] * v2.val.Int64Vector.v[i];
    }
}

void mult_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Float64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Float64Vector.v[i] * v2.val.Float64Vector.v[i];
    }
}

void mult_int_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Int64Vector.v[i] * v2.val.Float64Vector.v[i];
    }
}

void do_vector_mult(Variable v1, Variable v2, Variable *res) {
    if(is_int_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Int64Vector;
        res->val.Int64Vector.v = (int *)malloc(sizeof(int) * v1.val.Int64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        mult_int_vector(v1, v2, res);
    }
    
    if(is_int_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Int64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        mult_int_float_vector(v1, v2, res);
    }

    if(is_float_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        mult_int_float_vector(v2, v1, res);
    }

    if(is_float_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        mult_float_vector(v1, v2, res);
    }
}

void mult_int_scalar_vector(Variable v1, Variable v2, Variable *res) {
    if(is_int_vector(v2)) {
        res->type = Int64Vector;
        res->val.Int64Vector.n_elem = v2.val.Int64Vector.n_elem;
        res->val.Int64Vector.v = (int *)malloc(sizeof(int) * v2.val.Int64Vector.n_elem);
        for(int i = 0; i < res->val.Int64Vector.n_elem; i++) {
            res->val.Int64Vector.v[i] = v1.val.Int64 * v2.val.Int64Vector.v[i];
        }
    } else {
        res->type = Float64Vector;
        res->val.Float64Vector.n_elem = v2.val.Float64Vector.n_elem;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v2.val.Float64Vector.n_elem);
        for(int i = 0; i < res->val.Float64Vector.n_elem; i++) {
            res->val.Float64Vector.v[i] = v1.val.Int64 * v2.val.Float64Vector.v[i];
        }
    }
}

void mult_float_scalar_vector(Variable v1, Variable v2, Variable *res) {
    res->type = Float64Vector;
    if(is_int_vector(v2)) {
        res->val.Float64Vector.n_elem = v2.val.Int64Vector.n_elem;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v2.val.Int64Vector.n_elem);
        for(int i = 0; i < res->val.Float64Vector.n_elem; i++) {
            res->val.Float64Vector.v[i] = v1.val.Float64 * v2.val.Int64Vector.v[i];
        }
    } else {
        res->val.Float64Vector.n_elem = v2.val.Float64Vector.n_elem;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v2.val.Float64Vector.n_elem);
        for(int i = 0; i < res->val.Float64Vector.n_elem; i++) {
            res->val.Float64Vector.v[i] = v1.val.Float64 * v2.val.Float64Vector.v[i];
        }
    }
}

void do_mult_vector_scalar(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_vector(v2)) mult_int_scalar_vector(v1, v2, res);
    if(is_float(v1) && is_vector(v2)) mult_float_scalar_vector(v1, v2, res);
    
    if(is_vector(v1) && is_int(v2)) mult_int_scalar_vector(v2, v1, res);
    if(is_vector(v1) && is_float(v2)) mult_float_scalar_vector(v2, v1, res);
}

void do_mult(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_int(v2)) {
        res->type = Int64;
        res->val.Int64 = v1.val.Int64 * v2.val.Int64;
    } 
    
    if(is_string(v1) && is_string(v2)) {
        res->type = String;
        res->val.String = concat_string(v1, v2);
    }
    
    if(is_int(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Int64 * v2.val.Float64;
        res->type = Float64;
    }

    if(is_float(v1) && is_int(v2)) {
        res->val.Float64 = v1.val.Float64 * v2.val.Int64;
        res->type = Float64;
    }

    if(is_float(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Float64 * v2.val.Float64;
        res->type = Float64;
    }

    if(is_vector(v1) && is_int_or_float(v2) || is_int_or_float(v1) && is_vector(v2)) do_mult_vector_scalar(v1, v2, res);

    if(is_vector(v1) && is_vector(v2)) do_vector_mult(v1, v2, res);

}

void add_int_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Int64Vector.v[i] = v1.val.Int64Vector.v[i] + v2.val.Int64Vector.v[i];
    }
}

void add_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Float64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Float64Vector.v[i] + v2.val.Float64Vector.v[i];
    }
}

void add_int_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Int64Vector.v[i] + v2.val.Float64Vector.v[i];
    }
}

void sub_int_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Int64Vector.v[i] = v1.val.Int64Vector.v[i] - v2.val.Int64Vector.v[i];
    }
}

void sub_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Float64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Float64Vector.v[i] - v2.val.Float64Vector.v[i];
    }
}

void sub_int_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Int64Vector.v[i] - v2.val.Float64Vector.v[i];
    }
}

void do_vector_add(Variable v1, Variable v2, Variable *res) {
    if(is_int_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Int64Vector;
        res->val.Int64Vector.v = (int *)malloc(sizeof(int) * v1.val.Int64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        add_int_vector(v1, v2, res);
    } else if(is_int_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Int64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        add_int_float_vector(v1, v2, res);
    } else if(is_float_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        add_int_float_vector(v2, v1, res);
    } else if(is_float_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        add_float_vector(v1, v2, res);
    } else {
        error("Ilegal type in vector add");
    }
}

void do_vector_sub(Variable v1, Variable v2, Variable *res) {
    if(is_int_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Int64Vector;
        res->val.Int64Vector.v = (int *)malloc(sizeof(int) * v1.val.Int64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        sub_int_vector(v1, v2, res);
    } else if(is_int_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Int64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        sub_int_float_vector(v1, v2, res);
    } else if(is_float_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        sub_int_float_vector(v2, v1, res);
    } else if(is_float_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        sub_float_vector(v1, v2, res);
    } else {
        error("Ilegal type in sub vector");
    }
}

void do_sub(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_int(v2)) {
        res->type = Int64;
        res->val.Int64 = v1.val.Int64 - v2.val.Int64;
    } else if(is_int(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Int64 - v2.val.Float64;
        res->type = Float64;
    } else if(is_float(v1) && is_int(v2)) {
        res->val.Float64 = v1.val.Float64 - v2.val.Int64;
        res->type = Float64;
    } else if(is_float(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Float64 - v2.val.Float64;
        res->type = Float64;
    } else if(is_vector(v1) && is_vector(v2)) {
        do_vector_sub(v1, v2, res);
    } else {
        error("Ilegal type in sub");
    }

}

void do_add(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_int(v2)) {
        res->type = Int64;
        res->val.Int64 = v1.val.Int64 + v2.val.Int64;
        printf("Do_add1 %i %i\n", v1.type, v2.type);
    } else if(is_int(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Int64 + v2.val.Float64;
        res->type = Float64;
        printf("Do_add2 %i %i\n", v1.type, v2.type);
    } else if(is_float(v1) && is_int(v2)) {
        res->val.Float64 = v1.val.Float64 + v2.val.Int64;
        res->type = Float64;
        printf("Do_add3 %i %i\n", v1.type, v2.type);
    } else if(is_float(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Float64 + v2.val.Float64;
        res->type = Float64;
        printf("Do_add4 %i %i\n", v1.type, v2.type);
    } else if(is_vector(v1) && is_vector(v2)) {
        printf("Is vector %i %i\n", v1.type, v2.type);
        do_vector_add(v1, v2, res);
    } else {
        error("Ilegal type in add");
    }
}

bool is_string(Variable v1) {
    return v1.type == String;
}

char* concat_string(Variable v1, Variable v2) {
    int size_v1 = strlen(v1.val.String);
    int size_v2 = strlen(v2.val.String);
    char *concated = malloc(size_v1 + size_v2 + 1);
    strcpy(concated, v1.val.String);
    strcat(concated, v2.val.String);
    return concated;
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

void print_var(char* str, Variable var) {
        switch (var.type) {
            case Int64:
                if(DEBUG) printf("[DEBUG %s] Int64 %s = %i\n", str, var.var_name, var.val.Int64);
                else printf("Int64 %s = %i\n", var.var_name, var.val.Int64);
                break;
            case Float64:
                if(DEBUG) printf("[DEBUG %s] Float64 %s = %f\n", str, var.var_name, var.val.Float64);
                else printf("Float64 %s = %f\n", var.var_name, var.val.Float64);
                break;
            case String:
                if(DEBUG) printf("[DEBUG %s] String %s = %s\n", str, var.var_name, var.val.String);
                else printf("String %s = %s\n", var.var_name, var.val.String);
                break;
            case Bool:
                if(DEBUG) printf("[DEBUG %s] Bool %s = %i\n", str, var.var_name, var.val.Bool);
                else printf("Bool %s = %i\n", var.var_name, var.val.Bool);
                break;
            case Int64Vector:
                if(DEBUG) printf("[DEBUG %s] Int64Vector %s = \n", str, var.var_name);
                else printf("Int64Vector %s = \n", var.var_name);
                print_vector(var);
                break;
            case Float64Vector:
                if(DEBUG) printf("[DEBUG %s] Float64Vector %s = \n", str, var.var_name);
                else printf("Float64Vector %s = \n", var.var_name);
                print_vector(var);
                break;
            case Int64Matrix:
                if(DEBUG) printf("[DEBUG %s] Int64Matrix %s = \n", str, var.var_name);
                else printf("Int64Matrix %s = \n", var.var_name);
                print_matrix(var);
                break;
            case Float64Matrix:
                if(DEBUG) printf("[DEBUG %s] Float64Matrix %s = \n", str, var.var_name);
                else printf("Float64Matrix %s = \n", var.var_name);
                print_matrix(var);
                break;
            default:
                printf("Unknown type in print_var: %i\n", var.type);
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

void store_val(Variable var) {
    print_var("store_val", var);
    int ret = sym_add(var.var_name, &var);
    if(ret == SYMTAB_DUPLICATE) {
        ret = sym_remove(var.var_name);
        if(ret != SYMTAB_OK) symtab_error_handle("removing the var to store new val in store_val!", ret);
        ret = sym_add(var.var_name, &var);
    }
    if(ret != SYMTAB_OK) symtab_error_handle("storing value in symtab in store_val!", ret);
}

void get_val(char *key, Variable *v) {
    int ret = sym_lookup(key, v);
    if(ret != SYMTAB_OK) symtab_error_handle("loockup in show val!", ret);
}

void show_val(char *key) {
    Variable v;
    int ret = sym_lookup(key, &v);
    if(ret != SYMTAB_OK) symtab_error_handle("loockup in show val!", ret);
    print_var("show_val", v);
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

int get_val_int(Variable var) {
    return var.type == Int64 ? var.val.Int64 : var.val.Float64;
}

float get_val_float(Variable var) {
    return var.type == Int64 ? var.val.Int64 : var.val.Float64;
}

void store_matrix(NodeRow *row, Variable *var) {
    if(row->n_elem_row == 1 && row->val->n_elem_col == 1) var->type = row->row_type;
    else if(row->val->n_elem_col == 1 && row->row_type == Int64) var->type = Int64Vector;
    else if(row->val->n_elem_col == 1 && row->row_type == Float64) var->type = Float64Vector;
    else if(row->row_type == Int64) var->type = Int64Matrix;
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
                    if(c->n_elem_col != var->val.Int64Matrix.n_cols)  error("Error, matrix must be complete");
                    while(c != NULL) {
                        *(ptr++) = get_val_int(c->val);
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
                    if(c->n_elem_col != var->val.Float64Matrix.n_cols)  error("Error, matrix must be complete");
                    while(c != NULL) {
                        *(ptr++) = get_val_float(c->val);
                        c = c->next;
                    }
                    r = r->next;
                }
            }
            break;
        case Int64Vector: {
                var->val.Int64Vector.v = (int *) malloc(sizeof(int) * row->n_elem_row);
                int *ptr = var->val.Int64Vector.v;
                var->val.Int64Vector.n_elem = row->n_elem_row;
                while(r != NULL) {
                    if(r->val->n_elem_col != 1) error("Error, vector can't have more than one column!");
                    *(ptr++) = get_val_int(r->val->val);
                    r = r->next;
                }
            }
            break;
        case Float64Vector: {
                var->val.Float64Vector.v = (float *) malloc(sizeof(float) * row->n_elem_row);
                float *ptr = var->val.Float64Vector.v;
                var->val.Float64Vector.n_elem = row->n_elem_row;
                while(r != NULL) {
                    if(r->val->n_elem_col != 1) error("Error, vector can't have more than one column!");
                    *(ptr++) = get_val_float(r->val->val);
                    r = r->next;
                }
            }
            break;
        case Int64:
            var->val.Int64 = r->val->val.val.Int64;
            break;
        case Float64:
            var->val.Float64 = r->val->val.val.Float64;
            break;
    
    }
}