#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

#include "types.h"
#include "symtab.h"

//-------------errors handlers-------------
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

void error(char *str) {
    printf("%s\n", str);
    exit(1);
}

void printf_error(char *str, ...) {
    va_list args;

    va_start(args, str);
    vprintf(str, args);
    printf("\n");
    va_end(args);
    exit(1);
}


//-------------Type check-------------
bool is_int_vector(Variable v) {return v.type == Int64Vector;}

bool is_float_vector(Variable v) {return v.type == Float64Vector;}

bool is_int_matrix(Variable v) {return v.type == Int64Matrix;}

bool is_float_matrix(Variable v) {return v.type == Float64Matrix;}

bool is_int(Variable v) {return v.type == Int64;}

bool is_float(Variable v) {return v.type == Float64;}

bool is_int_or_float(Variable v1) {return is_int(v1) || is_float(v1);}

bool is_matrix(Variable v) {return v.type == Int64Matrix || v.type == Float64Matrix;}

bool is_vector(Variable v) {return is_int_vector(v) || is_float_vector(v);}

bool is_bool(Variable v) {return v.type == Bool;}

bool is_string(Variable v1) {return v1.type == String;}

bool is_literal(Variable v) {return !v.is_variable;}

bool is_variable(Variable v) {return v.is_variable;}

//-------------Val getters-------------
int get_val_int(Variable var) {
    return var.type == Int64 ? var.val.Int64 : var.val.Float64;
}

float get_val_float(Variable var) {
    return var.type == Int64 ? var.val.Int64 : var.val.Float64;
}

int get_var_string_len(Variable v) {
    if(is_variable(v)) return v.var_name_len + 1;

    switch (v.type) {
        case Int64:
        case Float64:
            return 12;
    }
}


//-------------Prints-------------
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

void fprint_matrix(Variable v, FILE *f) {
    switch (v.type) {
        case Int64Matrix:
            for(int i = 0; i < v.val.Int64Matrix.n_rows; i++) {
                for(int j = 0; j < v.val.Int64Matrix.n_cols; j++){
                    fprintf(f, "%i ", v.val.Int64Matrix.m[i * v.val.Int64Matrix.n_cols + j]);
                }
                fprintf(f, "\n");
            }
            break;
        case Float64Matrix:
            for(int i = 0; i < v.val.Float64Matrix.n_rows; i++) {
                for(int j = 0; j < v.val.Float64Matrix.n_cols; j++){
                    fprintf(f, "%f ", v.val.Float64Matrix.m[i * v.val.Float64Matrix.n_cols + j]);
                }
                fprintf(f, "\n");
            }
            break;
        default:
            error("Ilegal type in print_matrix");
            break;
    }
}

void print_vector(Variable v) {
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

void fprint_vector(Variable v, FILE *out) {
    switch (v.type) {
        case Int64Vector:
            for(int i = 0; i < v.val.Int64Vector.n_elem; i++) fprintf(out, "%i ", v.val.Int64Vector.v[i]);
            fprintf(out, "\n");
            break;
        case Float64Vector:
            for(int i = 0; i < v.val.Float64Vector.n_elem; i++) fprintf(out, "%f ", v.val.Float64Vector.v[i]);
            fprintf(out, "\n");
            break;
        default:
            error("Ilegal type in print_vector!");
            break;
    }
}

void print_var(char* str, Variable var, FILE *out) {
        switch (var.type) {
            case Int64:
                if(DEBUG) printf("[DEBUG %s] Int64 %s = %i\n", str, var.var_name, var.val.Int64);
                else {
                    printf("Int64 %s = %i\n", var.var_name, var.val.Int64);
                    fprintf(out, "Int64 %s = %i\n", var.var_name, var.val.Int64);
                }
                break;
            case Float64:
                if(DEBUG) printf("[DEBUG %s] Float64 %s = %f\n", str, var.var_name, var.val.Float64);
                else {
                    printf("Float64 %s = %f\n", var.var_name, var.val.Float64);
                    fprintf(out, "Float64 %s = %f\n", var.var_name, var.val.Float64);
                }
                break;
            case String:
                if(DEBUG) printf("[DEBUG %s] String %s = %s\n", str, var.var_name, var.val.String);
                else {
                    printf("String %s = %s\n", var.var_name, var.val.String);
                    fprintf(out, "String %s = %s\n", var.var_name, var.val.String);
                }
                break;
            case Bool:
                if(DEBUG) printf("[DEBUG %s] Bool %s = %i\n", str, var.var_name, var.val.Bool);
                else {
                    printf("Bool %s = %s\n", var.var_name, var.val.Bool ? "true" : "false");
                    fprintf(out, "Bool %s = %s\n", var.var_name, var.val.Bool ? "true" : "false");
                }
                break;
            case Int64Vector:
                if(DEBUG) printf("[DEBUG %s] Int64Vector %s = \n", str, var.var_name);
                else {
                    printf("Int64Vector %s = ", var.var_name);
                    fprintf(out, "Int64Vector %s = ", var.var_name);
                    print_vector(var);
                    fprint_vector(var, out);
                }
                break;
            case Float64Vector:
                if(DEBUG) printf("[DEBUG %s] Float64Vector %s = \n", str, var.var_name);
                else {
                    printf("Float64Vector %s = ", var.var_name);
                    fprintf(out, "Float64Vector %s = ", var.var_name);
                    print_vector(var);
                    fprint_vector(var, out);
                }
                break;
            case Int64Matrix:
                if(DEBUG) printf("[DEBUG %s] Int64Matrix %s = \n", str, var.var_name);
                else {
                    printf("Int64Matrix %s = \n", var.var_name);
                    fprintf(out, "Int64Matrix %s = \n", var.var_name);
                    print_matrix(var);
                    fprint_matrix(var, out);
                }
                break;
            case Float64Matrix:
                if(DEBUG) printf("[DEBUG %s] Float64Matrix %s = \n", str, var.var_name);
                else {
                    printf("Float64Matrix %s = \n", var.var_name);
                    fprintf(out, "Float64Matrix %s = \n", var.var_name);
                    print_matrix(var);
                    fprint_matrix(var, out);
                }
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


//-------------Symtab store, get-------------
void store_val(Variable var) {
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

void show_val(char *key, FILE *out) {
    Variable v;
    int ret = sym_lookup(key, &v);
    if(ret != SYMTAB_OK) symtab_error_handle("loockup in show val!", ret);
    print_var("show_val", v, out);
}

bool val_exists_in_symtab(char *key) {
    Variable v;
    return sym_lookup(key, &v) == SYMTAB_OK;
}

//-------------Get matrix, vector elems-------------
void get_id_matrix_elem(char *matrix_name, char *row_idx_name, char *col_idx_name, Variable *dst) {
    Variable row_idx, col_idx;
    get_val(row_idx_name, &row_idx);
    get_val(col_idx_name, &col_idx);
    if(row_idx.type != Int64 || col_idx.type != Int64) error("ilegal type in matrix index");
    get_matrix_elem(matrix_name, row_idx.val.Int64, col_idx.val.Int64, dst);
}

void get_matrix_elem(char *matrix_name, int row, int col, Variable *dst) {
    Variable m;
    get_val(matrix_name, &m);
    switch (m.type) {
        case Int64Matrix:
            dst->type = Int64;
            dst->val.Int64 = m.val.Int64Matrix.m[row * m.val.Int64Matrix.n_cols + col];
            break;
        case Float64Matrix:
            if(row < 0 || col < 0 || row > m.val.Float64Matrix.n_rows - 1 || col > m.val.Float64Matrix.n_cols - 1) error("Index out of bounds!");
            dst->type = Float64;
            dst->val.Int64 = m.val.Float64Matrix.m[row * m.val.Float64Matrix.n_cols + col];
            break;

        default:
            error("Ilegal type while trying to get an elem from a matrix!");
            break;
    }
}

void get_id_vector_elem(char *vector_name, char *vector_idx_name, Variable *res) {
    Variable index;
    get_val(vector_idx_name, &index);
    if(index.type != Int64) error("Ilegal type in vector idx");

    get_vector_elem(vector_name, index.val.Int64, res);
}

void get_vector_elem(char *vector_name, int idx, Variable *dst) {
    Variable v;
    get_val(vector_name, &v);
    switch (v.type) {
        case Int64Vector:
            if(idx < 0 || idx > v.val.Int64Vector.n_elem - 1) error("Index out of bounds!");
            dst->type = Int64;
            dst->val.Int64 = v.val.Int64Vector.v[idx];
            break;
        case Float64Vector:
            if(idx < 0 || idx > v.val.Float64Vector.n_elem - 1) error("Index out of bounds!");
            dst->type = Float64;
            dst->val.Int64 = v.val.Float64Vector.v[idx];
            break;

        default:
            error("Ilegal type while trying to get an elem from a vector!");
            break;
    }
}


//-------------Matrix treatment-------------
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
